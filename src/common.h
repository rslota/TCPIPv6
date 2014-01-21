#ifndef TCPIPStack_common_h
#define TCPIPStack_common_h


#include <stdint.h>
#include <stdio.h>

#define ETH_ADDR_LEN      6
#define ETH_PROTOCOL_IPV6 0x86DD
#define IP_ADDR_LEN       16

#define TCP_MAX_SEGEMENT_SIZE 1000
#define RECV_QUEUE_SIZE 1000

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define PACKED __attribute__((packed))

#define TCP_BUFFER_SIZE 1024*1024

typedef struct tcp_recv {
        uint32_t seq;
        size_t size;
        uint8_t  data[TCP_MAX_SEGEMENT_SIZE];
    } tcp_recv_t;

typedef struct tcp_session {

    tcp_recv_t* recv_queue[RECV_QUEUE_SIZE];

    uint32_t ack;
    uint32_t seq;
    uint32_t rcv_seq;
    uint8_t  state; 
    uint16_t port;
    uint16_t dst_port;
    uint8_t dst_ip[IP_ADDR_LEN];

    uint32_t send_buf_seq;
    uint32_t send_buf_end;
    uint8_t  send_buffer[TCP_BUFFER_SIZE];

    uint32_t recv_buf_end;
    uint32_t recv_buf_seq;
    uint8_t  recv_buffer[TCP_BUFFER_SIZE];

} tcp_session_t;

typedef struct session
{
    int session_id;
    uint8_t src_addr[ETH_ADDR_LEN];
    uint8_t src_ip[IP_ADDR_LEN];
    uint8_t protocol;
    uint16_t port;
    char *interface;
    tcp_session_t tcp;

    /// @todo There might be a better way to store this address.
    uint8_t last_sender_ip[IP_ADDR_LEN];
} session_t;


#endif
