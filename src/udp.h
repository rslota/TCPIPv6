/**
 * @file udp.h This file contains function definitions related to the UDP
 * protocol.
 */

#ifndef UDPIPStack_udp_h
#define UDPIPStack_udp_h


#include "common.h"
#include "ip.h"

#include <stddef.h>
#include <stdint.h>

#define UDP_HEADER_LEN       8
#define UDP_DATAGRAM_MAX_LEN 65507
#define UDP_DATA_MAX_LEN     (UDP_DATAGRAM_MAX_LEN - UDP_HEADER_LEN)

/**
 * Sends data through the network using the UDP protocol.
 * @param session The session object created by net_init().
 * @param dst_ip The destination IP address for the data.
 * @param dst_port The destination port for the data.
 * @param data The data to be sent.
 * @param data_len The length of data.
 * @returns Number of sent bytes on success, 0 on error.
 */
size_t udp_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port,
                const uint8_t data[], size_t data_len);

/**
 * Receives data from the network using the UDP protocol.
 * @param session The session object created by net_init().
 * @param buffer The buffer into which to write received data.
 * @param buffer_len The length of the buffer.
 * @returns Number of bytes written into the buffer on success, 0 on error.
 */
size_t udp_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif
