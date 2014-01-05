#ifndef UDPIPStack_udp_h
#define UDPIPStack_udp_h


#include "common.h"
#include "ip.h"

#include <stddef.h>
#include <stdint.h>

#define UDP_HEADER_LEN       8
#define UDP_DATAGRAM_MAX_LEN 65507
#define UDP_DATA_MAX_LEN     (UDP_DATAGRAM_MAX_LEN - UDP_HEADER_LEN)

size_t udp_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port, const uint8_t data[], size_t data_len);

size_t udp_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif