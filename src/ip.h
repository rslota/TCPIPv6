#ifndef TCPIPStack_ip_h
#define TCPIPStack_ip_h


#include "session.h"

#include <stddef.h>

#define IP_HEADER_LEN 40
#define IP_PACKET_MAX_LEN 1280
#define IP_DATA_MAX_LEN (IP_PACKET_MAX_LEN - IP_HEADER_LEN)

/**
 * Send data through the ip layer, to the ethernet layer.
 * @returns number of bytes of data that were sent on success, 0 on error.
 */
size_t ip_send(session_t *session, const uint8_t dst_addr[],
               const uint8_t data[], size_t data_len);


#endif
