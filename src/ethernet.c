#include "ethernet.h"

#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

#include <assert.h>
#include <memory.h>

typedef union PACKED eth_frame
{
    struct PACKED
    {
        uint8_t dst_addr[ETH_ADDR_LEN];
        uint8_t src_addr[ETH_ADDR_LEN];
        uint16_t ether_type;
        uint8_t data[ETH_DATA_MAX_LEN];
    };

    uint8_t buffer[ETH_FRAME_MAX_LEN];

} eth_frame_t;

size_t eth_send(session_t *session, const uint8_t dst_addr[],
                const uint8_t data[], size_t data_len)
{
    eth_frame_t frame;

    // Set the header
    memcpy(frame.dst_addr, dst_addr, ETH_ADDR_LEN);
    memcpy(frame.src_addr, session->src_addr, ETH_ADDR_LEN);
    frame.ether_type = htons(ETH_PROTOCOL_IPV6);

    // Set the data
    memset(frame.data, 0, ETH_DATA_MIN_LEN);
    memcpy(frame.data, data, data_len);

    // Prepare the sockaddr_ll struct, so the system knows which interface
    // should deal with the frame
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = session->ifindex;
    memcpy(addr.sll_addr, dst_addr, ETH_ADDR_LEN);
    addr.sll_halen = ETH_ADDR_LEN;

    // Send the frame: at least ETH_FRAME_MIN_LEN bytes, but no more than
    // ETH_FRAME_MAX_LEN.
    const size_t frame_len = MIN(ETH_FRAME_MAX_LEN,
                                 MAX(ETH_FRAME_MIN_LEN,
                                     ETH_HEADER_LEN + data_len));

    const size_t sent = sendto(session->sock_desc, frame.buffer, frame_len, 0,
                               (struct sockaddr*) &addr, sizeof(addr));

    return sent > ETH_HEADER_LEN ? sent - ETH_HEADER_LEN : 0;
}
