/**
 * @copyright Copyright © 2013-2014, Rafał Słota, Konrad Zemek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
 * @param session The session object created by net_init() with protocol
 * TCP_NOCONNECT.
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
