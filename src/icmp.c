#include "icmp.h"
#include "ip.h"
#include "hw.h"

size_t icmp_send(session_t *session, const uint8_t dst_ip[], uint8_t type, uint8_t code, const uint8_t body[], size_t message_len)
{
    icmp_packet_t message;
    message.type = type;
    message.code = code;

    message.checksum = 0;
    memcpy(message.body, body, message_len);

    message.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_ICMP, message.buffer, offsetof(icmp_packet_t, body) + message_len);

    printf("ICMP send... message_len: %d\n", message_len);
    return ip_send(session, dst_ip, IP_PROTOCOL_ICMP, (uint8_t*)&message, offsetof(icmp_packet_t, body) + message_len);
}

size_t icmp_recv(session_t *session, icmp_packet_t *packet)
{
    // TODO: validate checksum
    size_t recv = ip_recv(session, packet->buffer, sizeof(icmp_packet_t));
    if(recv > offsetof(icmp_packet_t, body)) {
        return recv - offsetof(icmp_packet_t, body);
    } else {
        return 0;
    }
}

void eui48_to_aarch(const uint8_t eth_addr[], uint8_t aarch_addr[])
{
    memcpy(aarch_addr, eth_addr, 3);
    memcpy(aarch_addr + 5, eth_addr + 3, 3);
    aarch_addr[3] = 0xff;
    aarch_addr[4] = 0xfe;
}

size_t ndp_solicitate_send(session_t *session, const uint8_t ip_addr[]) 
{
    uint8_t icmp_dest_addr[IP_ADDR_LEN] = { 0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xff, 0x0, 0x0, 0x0 };
    memcpy(icmp_dest_addr + 13, ip_addr + 13, 3);
    uint8_t link_addr_prefix[AARCH_ADDR_LEN] = { 0xfe, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    uint8_t aarch_addr[AARCH_ADDR_LEN];
    eui48_to_aarch(session->src_addr, aarch_addr);

    ndp_option_t option1;
    option1.type = NDP_SOURCE_LINK_ADDR_OPT;
    option1.len = 1;
    memcpy(option1.body, session->src_addr, ETH_ADDR_LEN);

    ndp_neighbor_discover_t ndp_query;
    memcpy(ndp_query.target_addr, ip_addr, IP_ADDR_LEN);
    memcpy(ndp_query.options, option1.buffer, offsetof(ndp_option_t, body) + ETH_ADDR_LEN);
    


    printf("NDP query...\n");
    size_t ret = icmp_send(session, icmp_dest_addr, ICMP_TYPE_NEIGHBOR_SOLICITATION, 0, ndp_query.buffer, (size_t)offsetof(ndp_neighbor_discover_t, options) + offsetof(ndp_option_t, body) + ETH_ADDR_LEN);
    printf("NDP query end...\n");

    return ret;
}

size_t ndp_advertisement_recv(session_t *session, ndp_neighbor_discover_t *ndp)
{
    icmp_packet_t icmp;
    size_t recv;

    while( (recv = icmp_recv(session, &icmp)) > 0 )
    {
        printf("torolol\n");
        if(icmp.type == ICMP_TYPE_NEIGHBOR_ADVERTISEMENT)
        {
            memcpy(ndp->buffer, icmp.body, recv);
            break;
        }
            
    }

    return recv;
}
