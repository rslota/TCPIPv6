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

    return ip_send(session, dst_ip, IP_PROTOCOL_UDP, datagram.buffer, datagram_len);
}

/// @todo check checksum
size_t udp_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    udp_datagram_t datagram;

    size_t received = 0;
    do
    {
        received = ip_recv(session, datagram.buffer, sizeof(datagram.buffer));
    } while(received != 0 && datagram.dst_port != (uint16_t)netb_s(session->port));

    const size_t data_len = MIN(received - UDP_HEADER_LEN, buffer_len);
    memcpy(buffer, datagram.data, data_len);
    return data_len;
}