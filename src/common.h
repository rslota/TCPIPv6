#ifndef TCPIPStack_common_h
#define TCPIPStack_common_h


#include <stdint.h>
#include <stdio.h>

#define ETH_ADDR_LEN      6
#define ETH_PROTOCOL_IPV6 0x86DD
#define IP_ADDR_LEN       16

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define PACKED __attribute__((packed))

typedef struct session
{
    int session_id;
    uint8_t src_addr[ETH_ADDR_LEN];
    uint8_t src_ip[IP_ADDR_LEN];
    uint8_t protocol;
    uint16_t port;
    char *interface;

    /// @todo There might be a better way to store this address.
    uint8_t last_sender_ip[IP_ADDR_LEN];
} session_t;


#endif
