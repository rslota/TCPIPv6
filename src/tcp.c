/**
 * @copyright Copyright © 2013-2014, Rafał Słota, Konrad Zemek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tcp.h"
#include "hw.h"
#include "net.h"
#include <string.h>
#include <stdlib.h>

#define FIN_FLAG (1 << 0)
#define SYN_FLAG (1 << 1)
#define RST_FLAG (1 << 2)
#define PSH_FLAG (1 << 3)
#define ACK_FLAG (1 << 4)

#define WORD_SIZE 4
#define TCP_MIN_HEADER_SIZE 5

#define TCP_MAX_SEGEMENT_SIZE 1000
#define RECV_QUEUE_SIZE 1000

#define TCP_STATE_LISTEN 1
#define TCP_STATE_SYN_SENT 2
#define TCP_STATE_SYN_RECEIVED 3
#define TCP_STATE_ESTABLISHED 4
#define TCP_STATE_CLOSED 5

typedef union PACKED tcp_header {
    struct PACKED {
        uint16_t src_port;
        uint16_t dest_port;
        uint32_t seq_num;
        uint32_t ack_num;
        uint8_t data_offset; // 4-bits - bata offset (x 32-bit words), 3-bit - reserved, 1-bit NS flag
        uint8_t flags;       // CWR, ECE, URG, ACK, PSH, RST, SYN, FIN
        uint16_t window_size;
        uint16_t checksum;
        uint16_t urg_pointer; // See URG flag
        uint32_t options[10]; // Maximum of 320 bits, divisible by 32
    };
    uint8_t buffer[TCP_MAX_SEGEMENT_SIZE];

} tcp_header_t;

void* send_worker(void *s)
{
    session_t *session = (session_t*) s;
    tcp_session_t *tcp = &session->tcp;
    tcp_header_t hdr;

    while(session->tcp.state == TCP_STATE_ESTABLISHED)
    {
        if(tcp->send_buf_seq >= tcp->send_buf_end)
        {
            sem_wait(&tcp->send_notify_sem);
            continue;
        }

        size_t to_send = MIN(500, tcp->send_buf_end - tcp->send_buf_seq);

        hdr.src_port = netb_s(tcp->port);
        hdr.dest_port = netb_s(tcp->dst_port);
        hdr.ack_num  = 0;
        hdr.seq_num = netb_l( tcp->seq );
        hdr.data_offset = TCP_MIN_HEADER_SIZE << 4;
        hdr.flags = 0;
        hdr.window_size = netb_s(1000);
        hdr.urg_pointer = 0;
        hdr.checksum = 0;

        memcpy(hdr.buffer + TCP_MIN_HEADER_SIZE * WORD_SIZE, tcp->send_buffer + tcp->send_buf_seq, to_send);

        hdr.checksum = ip_chksum(session, tcp->dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE + to_send);
        int res = ip_send(session, tcp->dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE + to_send);
        if(res > 0) {
            tcp->seq += to_send;
            tcp->send_buf_seq += to_send;
        }
    }

    return 0;
}

void* recv_worker(void *s)
{
    session_t *session = (session_t*) s;
    tcp_session_t *tcp = &session->tcp;
    size_t res;
    tcp_header_t rcv, hdr;

    for(int i = 0; i < RECV_QUEUE_SIZE; ++i)
        tcp->recv_queue[i] = 0;

    while(session->tcp.state == TCP_STATE_ESTABLISHED)
    {
        res = ip_recv(session, rcv.buffer, TCP_MAX_SEGEMENT_SIZE);
        if(res <= 0)
            continue;

        if(rcv.dest_port != netb_s(session->tcp.port) || rcv.src_port != netb_s(session->tcp.dst_port))
            continue;

        if(rcv.flags & RST_FLAG)
            continue;

        if(rcv.flags & ACK_FLAG)
        {
            int ack = hostb_l( rcv.ack_num );
            tcp->ack = ack;
            if(tcp->ack > tcp->seq)
                tcp->ack = tcp->seq;
        }

        uint32_t old_seq = tcp->rcv_seq;

        off_t d_offset = (rcv.data_offset >> 4) * WORD_SIZE;
        if(d_offset < res && res > TCP_MIN_HEADER_SIZE * WORD_SIZE)
        {

            uint8_t *data = rcv.buffer + d_offset;
            size_t d_len = res - d_offset;

            if(hostb_l(rcv.seq_num) == tcp->rcv_seq)
            {
                memcpy(tcp->recv_buffer + tcp->recv_buf_end, data, d_len);
                tcp->recv_buf_end += d_len;
                tcp->rcv_seq += d_len;
                sem_post(&tcp->recv_notify_sem);
            }
            else
            {
                for(int i = 0; i < RECV_QUEUE_SIZE; ++i)
                {
                    if(!tcp->recv_queue[i])
                    {
                        tcp->recv_queue[i] = malloc(sizeof(tcp_recv_t));
                        tcp->recv_queue[i]->size = d_len;
                        tcp->recv_queue[i]->seq = hostb_l(rcv.seq_num);
                        memcpy(tcp->recv_queue[i]->data, data, d_len);

                        break;
                    }
                }
            }

            for(int i = 0; i < RECV_QUEUE_SIZE; ++i)
            {
                if(!tcp->recv_queue[i])
                    continue;

                if(tcp->recv_queue[i]->seq == tcp->rcv_seq)
                {
                    memcpy(tcp->recv_buffer + tcp->recv_buf_end, tcp->recv_queue[i]->data, tcp->recv_queue[i]->size);
                    tcp->recv_buf_end += tcp->recv_queue[i]->size;
                    tcp->rcv_seq += tcp->recv_queue[i]->size;
                    sem_post(&tcp->recv_notify_sem);

                    free(tcp->recv_queue[i]);
                    tcp->recv_queue[i] = 0;
                }
            }

            if(tcp->rcv_seq > old_seq)
            {
                // Send ACK
                hdr.src_port = netb_s(tcp->port);
                hdr.dest_port = netb_s(tcp->dst_port);
                hdr.ack_num  = netb_l(tcp->rcv_seq);
                hdr.seq_num = netb_l( 1 );
                hdr.data_offset = TCP_MIN_HEADER_SIZE << 4;
                hdr.flags = ACK_FLAG;
                hdr.window_size = netb_s(1000);
                hdr.urg_pointer = 0;
                hdr.checksum = 0;

                hdr.checksum = ip_chksum(session, tcp->dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE);
                ip_send(session, tcp->dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE);
            }


        }
    }

    return 0;
}

size_t tcp_connect(session_t *session, const uint8_t dst_ip[], uint16_t dst_port)
{
    tcp_header_t hdr, rcv;

    static  int client_port = 16000;
    client_port = client_port + (rand() % 32000); /// @todo: random...


    session->tcp.send_buf_seq = session->tcp.send_buf_end = session->tcp.recv_buf_seq = session->tcp.recv_buf_end = 0;
    session->tcp.dst_port = dst_port;
    memcpy(session->tcp.dst_ip, dst_ip, IP_ADDR_LEN);

    // SYN
    hdr.src_port = netb_s(client_port);
    hdr.dest_port = netb_s(dst_port);
    hdr.ack_num  = netb_l(1);
    hdr.seq_num = netb_l(1);
    hdr.data_offset = TCP_MIN_HEADER_SIZE << 4;
    hdr.flags = SYN_FLAG;
    hdr.window_size = netb_s(1000);
    hdr.urg_pointer = 0;
    hdr.checksum = 0;

    hdr.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE);
    int res = ip_send(session, dst_ip, IP_PROTOCOL_TCP, hdr.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE);
    if(res <= 0) {
        return res;
    }

    session->tcp.state = TCP_STATE_SYN_SENT;
    session->tcp.port = client_port;

    // SYN-ACK
    while((res = ip_recv(session, rcv.buffer, TCP_MIN_HEADER_SIZE * WORD_SIZE)) > 0)
    {
        if((rcv.flags & ACK_FLAG) && (rcv.flags & SYN_FLAG)
            && rcv.ack_num == netb_l(hostb_l(hdr.seq_num) + 1)
            && rcv.dest_port == netb_s(session->tcp.port)
            && rcv.src_port == netb_s(dst_port))
        {
            break;
        }
    }

    if(res <= 0)
    {
        return res;
    }

    session->tcp.seq = hostb_l(rcv.ack_num);

    // ACK
    hdr.seq_num = rcv.ack_num;
    hdr.ack_num = netb_l(hostb_l(rcv.seq_num) + 1);
    hdr.flags = ACK_FLAG;
    hdr.checksum = 0;
    hdr.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_TCP, hdr.buffer, 20);

    res = ip_send(session, dst_ip, IP_PROTOCOL_TCP, hdr.buffer, (hdr.data_offset >> 4) * 4);
    if(res <= 0) {
        return res;
    }

    session->tcp.ack = hostb_l(hdr.ack_num);
    session->tcp.rcv_seq = hostb_l(rcv.seq_num) + 1;

    if(sem_init(&session->tcp.recv_notify_sem, 0, 0) != 0)
        return 0;

    session->tcp.state = TCP_STATE_ESTABLISHED;

    thread_spawn(&recv_worker, (void*)session);
    thread_spawn(&send_worker, (void*)session);

    return 1;
}

session_t *tcp_listen(session_t *session, const uint8_t bind_ip[], uint16_t bind_port)
{
    int res;
    tcp_header_t hdr, rcv;

    session->tcp.state = TCP_STATE_LISTEN;
    session->tcp.send_buf_seq = session->tcp.send_buf_end = session->tcp.recv_buf_seq = session->tcp.recv_buf_end = 0;

    // SYN
    while((res = ip_recv(session, rcv.buffer, 20)) > 0)
    {
        if((rcv.flags & SYN_FLAG) && !(rcv.flags & ACK_FLAG) && rcv.dest_port == netb_s(bind_port))
        {
            break;
        }
    }

    if(res <= 0)
        return 0;

    // SYN-ACK
    hdr.src_port = netb_s(bind_port);
    hdr.dest_port = rcv.src_port;
    hdr.seq_num = rcv.seq_num;
    hdr.ack_num = netb_l(hostb_l(rcv.seq_num) + 1);
    hdr.data_offset = 5 << 4;
    hdr.flags = SYN_FLAG | ACK_FLAG;
    hdr.window_size = netb_s(1000);
    hdr.urg_pointer = 0;
    hdr.checksum = 0;
    hdr.checksum = ip_chksum(session, session->last_sender_ip, IP_PROTOCOL_TCP, hdr.buffer, 20);

    session_t *nsess = net_init(session->interface, session->src_ip, bind_port, 0, 0, TCP_NOCONNECT, -1);

    res = ip_send(session, session->last_sender_ip, IP_PROTOCOL_TCP, hdr.buffer, (hdr.data_offset >> 4) * 4);
    if(res <= 0) {
        return 0;
    }

    session->tcp.rcv_seq = hostb_l(rcv.seq_num) + 1;
    session->tcp.dst_port = hostb_s(rcv.src_port);
    memcpy(session->tcp.dst_ip, session->last_sender_ip, IP_ADDR_LEN);

    // ACK
    while((res = ip_recv(session, rcv.buffer, 20)) > 0)
    {
        if(!(rcv.flags & SYN_FLAG) && (rcv.flags & ACK_FLAG)
            && rcv.dest_port == netb_s(bind_port)
            && rcv.ack_num == netb_l(hostb_l(hdr.seq_num) + 1)
            && rcv.seq_num == hdr.ack_num)
        {
            break;
        }
    }
    if(res <= 0) {
        return 0;
    }

    nsess->tcp.state = TCP_STATE_ESTABLISHED;
    nsess->tcp.ack = hostb_l(rcv.seq_num) + 1;
    nsess->tcp.seq = hostb_l(hdr.seq_num) + 1;
    nsess->tcp.send_buf_seq = nsess->tcp.send_buf_end = nsess->tcp.recv_buf_seq = nsess->tcp.recv_buf_end = 0;
    nsess->tcp.rcv_seq = hostb_l(hdr.seq_num) + 1;
    nsess->tcp.dst_port = hostb_s(rcv.src_port);
    nsess->tcp.port = nsess->port;
    memcpy(nsess->tcp.dst_ip, session->last_sender_ip, IP_ADDR_LEN);

    if(sem_init(&nsess->tcp.recv_notify_sem, 0, 0) != 0)
        return 0;

    thread_spawn(&recv_worker, (void*)nsess);
    thread_spawn(&send_worker, (void*)nsess);

    return nsess;
}

size_t tcp_send(session_t *session, const uint8_t data[], size_t data_len)
{
    if(session->tcp.state != TCP_STATE_ESTABLISHED)
        return 0;

    if( session->tcp.send_buf_end + data_len >= TCP_BUFFER_SIZE )
    {
        uint8_t *real_ack = session->tcp.send_buffer + session->tcp.send_buf_seq - (session->tcp.seq - session->tcp.ack);
        off_t shift = real_ack - session->tcp.send_buffer;
        memmove(session->tcp.send_buffer, real_ack, shift);
        session->tcp.send_buf_end -= shift;
        session->tcp.send_buf_seq -= shift;
    }

    size_t queued = data_len;
    if(TCP_BUFFER_SIZE - session->tcp.send_buf_end < queued)
        queued = TCP_BUFFER_SIZE - session->tcp.send_buf_end ;
    memcpy(session->tcp.send_buffer + session->tcp.send_buf_end, data, queued);
    session->tcp.send_buf_end += queued;
    sem_post(&session->tcp.send_notify_sem);

    return queued;
}

size_t tcp_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    if(session->tcp.state != TCP_STATE_ESTABLISHED)
        return 0;

    while(session->tcp.recv_buf_seq >= session->tcp.recv_buf_end)
    {
        sem_wait(&session->tcp.recv_notify_sem);
    }

    size_t recv_size = MIN(buffer_len, session->tcp.recv_buf_end - session->tcp.recv_buf_seq);
    memcpy(buffer, session->tcp.recv_buffer + session->tcp.recv_buf_seq, recv_size);
    session->tcp.recv_buf_seq += recv_size;

    return recv_size;
}

size_t tcp_close(session_t *session)
{
    if(session->tcp.state != TCP_STATE_ESTABLISHED)
        return 0;

    sem_destroy(&session->tcp.recv_notify_sem);
    session->tcp.state = TCP_STATE_CLOSED;

    return 1;
}

