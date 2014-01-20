#ifndef TCPIPStack_tcp_h
#define TCPIPStack_tcp_h


#include "common.h"
#include "ip.h"

#include <stddef.h>
#include <stdint.h>


size_t tcp_connect(session_t *session, const uint8_t dst_ip[], uint16_t dst_port);

size_t tcp_close(session_t *session);

session_t *tcp_listen(session_t *session, const uint8_t bind_ip[], uint16_t bind_port);

size_t tcp_send(session_t *session, const uint8_t data[], size_t data_len);

size_t tcp_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif // TCPIPStack_tcp_h
