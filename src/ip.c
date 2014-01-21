#include "ip.h"

#include "eth.h"
#include "hw.h"
#include "icmp.h"
#include "ndp_daemon.h"
#include "net.h"

#include <memory.h>

typedef union PACKED ip_packet
{
    union PACKED
    {
        struct PACKED
        {
            uint32_t flow; // 4b version, 8b traffic class, 20b flow id
            uint16_t payload_length;
            uint8_t  next_header;
            uint8_t  hop_limit;
            uint8_t  src_ip[IP_ADDR_LEN];
            uint8_t  dst_ip[IP_ADDR_LEN];
            uint8_t  data[IP_DATA_MAX_LEN];
        };

        uint8_t version; // The first 4 bits are the version
    };

    uint8_t buffer[IP_PACKET_MAX_LEN];

} ip_packet_t;

static int ip_to_hw(session_t *session, const uint8_t ip_addr[],
                    uint8_t hw_addr[])
{
    // Multicast addr case:
    if(ip_addr[0] == 0xff)
    {
        static uint8_t multicast_addr[] = { 0x33, 0x33, 0x0, 0x0, 0x0, 0x0 };

        // Multicast mapping: http://tools.ietf.org/html/rfc2464#page-4
        memcpy(multicast_addr + 2, ip_addr + IP_ADDR_LEN - 4, 4);
        memcpy(hw_addr, multicast_addr, ETH_ADDR_LEN);

        return 0;
    }

    // Localhost hardwire:
    if(memcmp(ip_addr, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1", IP_ADDR_LEN) == 0 ||
       memcmp(ip_addr, session->src_ip, IP_ADDR_LEN) == 0)
    {
        memcpy(hw_addr, session->src_addr, ETH_ADDR_LEN);
        return 0;
    }

    if(ndp_table_lookup(ip_addr, hw_addr))
        return 0;

    session_t *icmp_session = net_init(session->interface, session->src_ip, 0,
                                       0, 0, ICMP, 1000);

    ndp_neighbor_discover_t ndp;
    size_t recv;
    ndp_solicitate_send(icmp_session, ip_addr);

    while( (recv = ndp_advertisement_recv(icmp_session, &ndp) ) > 0)
    {
        if( netb_l( ndp.reserved ) & 1 << 30)
            break;
    }

    if(!recv)
        return -1;

    ndp_option_t option;
    memcpy(option.buffer, ndp.options, recv - NDP_ND_HEADER_LEN);
    if(option.type != NDP_TARGET_LINK_ADDR_OPT || option.len != 1)
    {
        fprintf(stderr, "Error: unsupported option type for NDP protocol: %d "
                        "with len: %d\n", option.type, option.len);
        return -1;
    }

    net_free(icmp_session);

    memcpy(hw_addr, option.body, ETH_ADDR_LEN);

    ndp_table_insert(ip_addr, hw_addr);

    return 0;
}

size_t ip_send(session_t *session, const uint8_t dst_ip[], uint8_t protocol,
               const uint8_t data[], size_t data_len)
{
    // We can send maximum of IP_DATA_MAX_LEN bytes of data.
    if(data_len > IP_DATA_MAX_LEN)
        return 0;

    // Prepare the packet
    ip_packet_t packet;
    packet.flow = 0;
    packet.version = 0x60;
    packet.payload_length = netb_s(data_len);
    packet.next_header = protocol;
    packet.hop_limit = 255;
    memcpy(packet.src_ip, session->src_ip, IP_ADDR_LEN);
    memcpy(packet.dst_ip, dst_ip, IP_ADDR_LEN);
    memcpy(packet.data, data, data_len);


    uint8_t dst_hw_addr[ETH_ADDR_LEN];
    if(ip_to_hw(session, dst_ip, dst_hw_addr) != 0)
        return 0;

    const size_t packet_len = IP_HEADER_LEN + data_len;
    const size_t sent = eth_send(session, dst_hw_addr, packet.buffer,
                                 packet_len);

    return sent == packet_len ? data_len : 0;
}

size_t ip_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    ip_packet_t packet;
    static uint8_t
        icmp_multicast_addr[IP_ADDR_LEN] = { 0xff, 0x02, 0x0, 0x0, 0x0, 0x0,
                                             0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xff,
                                             0x0, 0x0, 0x0 };

    memcpy(icmp_multicast_addr + 13, session->src_ip + 13, 3);

    size_t received;
    // Skip all packets that don't match the session's protocol or ip_addr
    while( (received = eth_recv(session, packet.buffer)) > 0 )
    {
        if(packet.next_header == session->protocol &&
            (memcmp(packet.dst_ip, session->src_ip, IP_ADDR_LEN) == 0 ||
             memcmp(packet.dst_ip, icmp_multicast_addr, IP_ADDR_LEN) == 0 ) )
            break;
    }

    if(received == 0)
        return 0;

    const size_t data_len = MIN(received - IP_HEADER_LEN, buffer_len);
    memcpy(buffer, packet.data, data_len);

    memcpy(session->last_sender_ip, packet.src_ip, IP_ADDR_LEN);

    return data_len;
}

#define PSEUDO_PACKET_HEADER_LEN (2 * IP_ADDR_LEN + 8)

// Copying the whole packet data in order to calculate the checksum is far
// from the most efficient implementation, but it's one of the most convenient
// ones.
typedef union PACKED pseudo_packet
{
    struct PACKED
    {
        uint8_t src_ip[IP_ADDR_LEN];
        uint8_t dst_ip[IP_ADDR_LEN];
        uint32_t upper_layer_packet_len;
        uint8_t zeros[3];
        uint8_t next_header;
        uint8_t data[IP_DATA_MAX_LEN];
    };

    uint16_t buffer[(PSEUDO_PACKET_HEADER_LEN + IP_DATA_MAX_LEN)/2];

} pseudo_packet_t;


// Inefficient. It's better to accumulate in uint32_t value, and
// continue adding first 2 bytes to last 2 bytes until the first 2 bytes are 0
// (which is at most 2 times). On the other hand, the checksumming is normally
// done by the hardware (checksum offloading).
static uint16_t add_with_carry(uint16_t acc, uint16_t val)
{
    // The condition in ternary op checks for overflow without casting.
    return (acc + val) + (acc > UINT16_MAX - val ? 1 : 0);
}

uint16_t ip_chksum(session_t *session, const uint8_t dst_ip[], uint8_t protocol,
                   const uint8_t data[], size_t data_len)
{
    pseudo_packet_t packet;
    memcpy(packet.src_ip, session->src_ip, IP_ADDR_LEN);
    memcpy(packet.dst_ip, dst_ip, IP_ADDR_LEN);
    packet.upper_layer_packet_len = netb_l(data_len);
    memset(packet.zeros, 0, sizeof(packet.zeros));
    packet.next_header = protocol;
    memcpy(packet.data, data, data_len);

    // Pad the data with 0s so that data_len is even
    if(data_len % 2 != 0)
    {
        packet.data[data_len] = 0;
        data_len += 1;
    }

    uint32_t acc = 0;
    for(int i = 0; i < (PSEUDO_PACKET_HEADER_LEN + data_len)/2; ++i)
        acc = add_with_carry(acc, packet.buffer[i]);

    return (~acc == 0 ? acc : ~acc);
}
