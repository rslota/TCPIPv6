#ifndef TCPIPStack_tcp_h
#define TCPIPStack_tcp_h


#include "common.h"

#include <stddef.h>
#include <stdint.h>

size_t tcp_send(session_t *session, const uint8_t data[], size_t data_len);

size_t tcp_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif