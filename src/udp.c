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

#include "udp.h"

#include "hw.h"
#include "ip.h"

#include <memory.h>

typedef union PACKED udp_datagram
{
    struct PACKED
    {
        uint16_t src_port;
        uint16_t dst_port;
        uint16_t datagram_len;
        uint16_t checksum;
        uint8_t data[UDP_DATA_MAX_LEN];
    };

    uint8_t buffer[UDP_DATAGRAM_MAX_LEN];

} udp_datagram_t;

size_t udp_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port,
                const uint8_t data[], size_t data_len)
{
    udp_datagram_t datagram;

    const size_t datagram_len = UDP_HEADER_LEN + data_len;

    memcpy(datagram.data, data, data_len);

    datagram.src_port = netb_s(session->port);
    datagram.dst_port = netb_s(dst_port);
    datagram.datagram_len = netb_s(datagram_len);
    datagram.checksum = 0;

    datagram.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_UDP,
                                  datagram.buffer, datagram_len);

    return ip_send(session, dst_ip, IP_PROTOCOL_UDP, datagram.buffer,
                   datagram_len);
}

/// @todo Check checksum
size_t udp_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    udp_datagram_t datagram;

    size_t received = 0;
    do
    {
        received = ip_recv(session, datagram.buffer, sizeof(datagram.buffer));
    } while(received != 0 && datagram.dst_port != netb_s(session->port));

    const size_t data_len = MIN(received - UDP_HEADER_LEN, buffer_len);
    memcpy(buffer, datagram.data, data_len);
    return data_len;
}
