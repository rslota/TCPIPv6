#ifndef TCPIPStack_net_h
#define TCPIPStack_net_h


#include "common.h"

#include <stdint.h>
#include <stddef.h>

typedef enum protocol
{
	TCP,
	UDP
} protocol_t;

/**
 * Opens the session.
 * @returns pointer to the created session on success, 0 on error.
 */
session_t *net_init(const char *interface, const uint8_t src_ip[],
                    uint16_t port, protocol_t protocol);

/**
 * Closes the session.
 * @returns 0 on success, -1 on error.
 */
int net_free(session_t *session);

size_t net_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port,
                const uint8_t data[], size_t data_len);

size_t net_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif
