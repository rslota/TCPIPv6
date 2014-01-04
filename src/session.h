#ifndef TCPIPStack_session_h
#define TCPIPStack_session_h


#include "common.h"

#include <stdint.h>

typedef struct session
{
    int session_id;
    uint8_t src_addr[ETH_ADDR_LEN];
    uint8_t src_ip[IP_ADDR_LEN];
    uint8_t protocol;
    uint16_t port;

} session_t;

typedef enum protocol
{
	TCP = IP_PROTOCOL_TCP,
	UDP = IP_PROTOCOL_UDP
} protocol_t;

/**
 * Opens the session.
 * @returns pointer to the created session on success, 0 on error.
 */
session_t *session_open(const char *interface, const uint8_t src_ip[], uint16_t port, protocol_t protocol);

/**
 * Closes the session.
 * @returns 0 on success, -1 on error.
 */
int session_close(session_t *session);


#endif
