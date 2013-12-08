#include "ip.h"

#include "common.h"
#include "ethernet.h"

#include <arpa/inet.h>

#include <memory.h>

typedef union PACKED ip_packet
{
    union PACKED
    {
        struct PACKED
        {
            uint32_t flow; // 4 bits version, 8 bits traffic class, 20 bits flow id
            uint16_t payload_length;
            uint8_t  next_header;
            uint8_t  hop_limit;
            uint8_t  src_addr[IP_ADDR_LEN];
            uint8_t  dst_addr[IP_ADDR_LEN];
            uint8_t  data[IP_DATA_MAX_LEN];
        };

        uint8_t version; // The first 4 bits are the version
    };

    uint8_t buffer[IP_PACKET_MAX_LEN];

} ip_packet_t;

static int ip_to_hw(const uint8_t ip_addr[], uint8_t hw_addr[])
{
    static uint8_t addr[] = { 0x33, 0x33, 0x0, 0x0, 0x0, 0x0 };
    memcpy(hw_addr, addr, ETH_ADDR_LEN);
    return 0;
}

size_t ip_send(session_t *session, const uint8_t dst_addr[],
               const uint8_t data[], size_t data_len)
{
    // We can send maximum of IP_DATA_MAX_LEN
    data_len = MIN(data_len, IP_DATA_MAX_LEN);

    // Prepare the packet
    ip_packet_t packet;
    packet.flow = htonl(0);
    packet.version = 0x60;
    packet.payload_length = htons(data_len);
    packet.next_header = 59; // no next header
    packet.hop_limit = 64;
    memcpy(packet.src_addr, session->src_ip, IP_ADDR_LEN);
    memcpy(packet.dst_addr, dst_addr, IP_ADDR_LEN);
    memcpy(packet.data, data, data_len);

    uint8_t dst_hw_addr[ETH_ADDR_LEN];
    if(ip_to_hw(dst_addr, dst_hw_addr) != 0)
        return 0;

    const size_t packet_len = IP_HEADER_LEN + data_len;
    const size_t sent = eth_send(session, dst_hw_addr, packet.buffer, packet_len);

    return sent == packet_len ? data_len : 0;
}
