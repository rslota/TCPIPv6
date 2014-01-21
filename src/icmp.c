#include "icmp.h"

#include "hw.h"
#include "ip.h"

#include <memory.h>

size_t icmp_send(session_t *session, const uint8_t dst_ip[], uint8_t type,
                 uint8_t code, const uint8_t body[], size_t body_len)
{
    icmp_packet_t message;
    message.type = type;
    message.code = code;

    message.checksum = 0;
    memcpy(message.body, body, body_len);

    const size_t message_len = ICMP_HEADER_LEN + body_len;

    message.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_ICMP,
                                 message.buffer, message_len);

    const size_t sent = ip_send(session, dst_ip, IP_PROTOCOL_ICMP,
                                message.buffer, message_len);

    return sent == message_len ? sent : 0;
}

size_t icmp_recv(session_t *session, icmp_packet_t *packet)
{
    /// @todo Validate checksum
    const size_t recv = ip_recv(session, packet->buffer,
                                sizeof(packet->buffer));

    return recv > ICMP_HEADER_LEN ? recv - ICMP_HEADER_LEN : 0;
}

size_t ndp_solicitate_send(session_t *session, const uint8_t ip_addr[])
{
    uint8_t icmp_dest_addr[IP_ADDR_LEN] = { 0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0,
                                            0x0, 0x0, 0x0, 0x0, 0x1, 0xff, 0x0,
                                            0x0, 0x0 };

    memcpy(icmp_dest_addr + 13, ip_addr + 13, 3);

    ndp_option_t option1;
    option1.type = NDP_SOURCE_LINK_ADDR_OPT;
    option1.len = 1;
    memcpy(option1.body, session->src_addr, ETH_ADDR_LEN);

    const size_t opt_len = NDP_OPT_HEADER_LEN + ETH_ADDR_LEN;
    const size_t nd_len = NDP_ND_HEADER_LEN + opt_len;

    ndp_neighbor_discover_t ndp_query;
    memcpy(ndp_query.target_addr, ip_addr, IP_ADDR_LEN);
    memcpy(ndp_query.options, option1.buffer, opt_len);

    const size_t sent = icmp_send(session, icmp_dest_addr,
                                  ICMP_TYPE_NEIGHBOR_SOLICITATION, 0,
                                  ndp_query.buffer, nd_len);

    return sent == nd_len ? sent : 0;
}

size_t ndp_advertisement_recv(session_t *session, ndp_neighbor_discover_t *ndp)
{
    icmp_packet_t icmp;
    size_t recv = 0;

    while( (recv = icmp_recv(session, &icmp)) > 0 )
    {
        if(icmp.type == ICMP_TYPE_NEIGHBOR_ADVERTISEMENT)
        {
            memcpy(ndp->buffer, icmp.body, recv);
            break;
        }
    }

    return recv;
}
