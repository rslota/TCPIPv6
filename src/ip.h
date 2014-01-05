#ifndef TCPIPStack_ip_h
#define TCPIPStack_ip_h


#include "common.h"

#include <stdint.h>
#include <stddef.h>

#define IP_HEADER_LEN     40
#define IP_PACKET_MAX_LEN 1280
#define IP_DATA_MAX_LEN   (IP_PACKET_MAX_LEN - IP_HEADER_LEN)
#define IP_PROTOCOL_ICMP  1
#define IP_PROTOCOL_TCP   6
#define IP_PROTOCOL_UDP   17

/**
 * Send data through the ip layer, to the ethernet layer.
 * @returns number of bytes of data that were sent on success, 0 on error.
 */
size_t ip_send(session_t *session, const uint8_t dst_ip[], uint8_t protocol,
               const uint8_t data[], size_t data_len);

/**
 * Receive data from the ethernet layer, through the ip layer.
 * @param buffer the array to which the data will be written
 * @returns number of bytes written into data array, 0 on error.
 */
size_t ip_recv(session_t *session, uint8_t buffer[], const size_t buffer_len);

/**
 * Calculate the checksum used by some transport layer protocols.
 * @param protocol the protocol number corresponding to the used transport
 * layer protocol.
 * @param ip_data the whole payload of the resulting IP packet, except any
 * extension headers; the checksum field must be 0. The data needs to be in the
 * network byte order.
 * @returns the calculated checksum to place in transport layer's header; the
 * checksum is already in the network byte order.
 */
uint16_t ip_chksum(session_t *session, const uint8_t dst_ip[], uint8_t protocol,
                   uint8_t data[], size_t data_len);


#endif
