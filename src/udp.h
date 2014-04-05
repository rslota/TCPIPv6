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
