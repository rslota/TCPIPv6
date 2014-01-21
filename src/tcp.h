/**
 * @file tcp.h This file contains function definitions related to the TCP
 * protocol.
 */


#ifndef TCPIPStack_tcp_h
#define TCPIPStack_tcp_h


#include "common.h"

#include "ip.h"

#include <stddef.h>
#include <stdint.h>

/**
 * Initiates a TCP connection.
 * @param session The session object created by net_init().
 * @param dst_ip The IP address of the remote host.
 * @param dst_port The TCP port of the remote host.
 * @returns non-0 value on success, otherwise 0.
 */
size_t tcp_connect(session_t *session, const uint8_t dst_ip[],
                   uint16_t dst_port);

/**
 * Closes a TCP connection.
 * @param session The session object created by net_init() on the client side;
 * the session object associated with the connection for the server side.
 * @returns non-0 value on success, otherwise 0.
 */
size_t tcp_close(session_t *session);

/**
 * Waits for a connection from a remote host.
 * @param session The session object created by net_init().
 * @param bind_ip The IP the server will be bound to.
 * @param bind_port The TCP port the server will be bound to.
 * @returns The new session associated with the connection.
 */
session_t *tcp_listen(session_t *session, const uint8_t bind_ip[],
                      uint16_t bind_port);

/**
 * Sends data through the network using the TCP protocol.
 * @param session The session object created by net_init().
 * @param data The data to be sent.
 * @param data_len The length of data.
 * @returns Number of sent bytes on success, 0 on error.
 */
size_t tcp_send(session_t *session, const uint8_t data[], size_t data_len);

/**
 * Receives data from the network using the TCP protocol.
 * @param session The session object created by net_init().
 * @param buffer The buffer into which to write received data.
 * @param buffer_len The length of the buffer.
 * @returns Number of bytes written into the buffer on success, 0 on error.
 */
size_t tcp_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif // TCPIPStack_tcp_h
