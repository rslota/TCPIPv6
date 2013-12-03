#ifndef TCPIPStack_session_details_h
#define TCPIPStack_session_details_h


#include "common.h"

#include <stdint.h>

typedef struct session
{
    int ifindex;
    uint8_t src_addr[ETH_ADDR_LEN];
    int sock_desc;

} session_t;

session_t *session_open(const char *ifname);

int session_close(session_t *session);


#endif
