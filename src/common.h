#ifndef TCPIPStack_common_h
#define TCPIPStack_common_h


#include <stdint.h>
#include <stdlib.h>

#define ETH_ADDR_LEN      6
#define ETH_PROTOCOL_IPV6 0x86DD

#define IP_ADDR_LEN      16
#define IP_PROTOCOL_ICMP 1
#define IP_PROTOCOL_TCP  6
#define IP_PROTOCOL_UDP  17

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define PACKED __attribute__((packed))


#endif
