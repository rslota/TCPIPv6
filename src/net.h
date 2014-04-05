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
 * @file net.h This file contains function definitions for the external
 * interface of the TCP/IP stack.
 */

#ifndef TCPIPStack_net_h
#define TCPIPStack_net_h


#include "common.h"

#include <stdint.h>
#include <stddef.h>

typedef enum protocol
{
	TCP,
    TCP_NOCONNECT,
	UDP,
    ICMP
} protocol_t;

/**
 * Initializes a new session.
 * @param interface The name of an interface used; the value varies depending
 * on the platform.
 * @param src_ip_addr The IP address to use as a source address.
 * @param src_port The port number to use as a source port for certain
 * protocols.
 * @param dst_ip_addr The IP address of the remote host; used only for the TCP
 * protocol.
 * @param dst_port The port number of the remote host; used only for the TCP
 * protocol.
 * @param protocol The protocol to use in the session.
 * @param recv_timeout The timeout on receive operation in milliseconds; -1
 * means infinity.
 * @returns A pointer to the created session on success, 0 on error.
 */
session_t *net_init(const char *interface, const uint8_t src_ip_addr[],
                    uint16_t src_port, const uint8_t dst_ip_addr[],
                    uint16_t dst_port, protocol_t protocol, int recv_timeout);

/**
 * Frees a session.
 * @param session The session object created by net_init().
 * @returns 0 on success, -1 on error.
 */
int net_free(session_t *session);

/**
 * Sends data through the network.
 * @param session The session object created by net_init().
 * @param dst_ip The destination IP address for the data. Ignored for TCP
 * protocol.
 * @param dst_port The destination port for the data for certain protocols.
 * @param data The data to send through the network.
 * @param data_len The length of data.
 * @returns Number of sent bytes on success, 0 on error.
 */
size_t net_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port,
                const uint8_t data[], size_t data_len);

/**
 * Receives data from the network.
 * @param session The session object created by net_init().
 * @param buffer The buffer into which to write received data.
 * @param buffer_len The length of the buffer.
 * @returns Number of bytes written into the buffer on success, 0 on error.
 */
size_t net_recv(session_t *session, uint8_t buffer[], size_t buffer_len);


#endif
