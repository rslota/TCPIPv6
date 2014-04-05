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
 * @file eth.h This file contains functions and definitions connected to the
 * OSI layer 2 functionality.
 */

#ifndef TCPIPStack_eth_h
#define TCPIPStack_eth_h


#include "common.h"

#include <stddef.h>
#include <stdint.h>

#define ETH_HEADER_LEN    14
#define ETH_DATA_MIN_LEN  46
#define ETH_DATA_MAX_LEN  1500
#define ETH_FRAME_MIN_LEN (ETH_HEADER_LEN + ETH_DATA_MIN_LEN)
#define ETH_FRAME_MAX_LEN (ETH_HEADER_LEN + ETH_DATA_MAX_LEN)

/**
 * Sends data through the ethernet layer, to the hardware layer.
 * @param session The session object created by net_init().
 * @param dst_addr The destination MAC address.
 * @param data The data to send through the ethernet layer.
 * @param data_len The length of data.
 * @returns Number of bytes of data that were sent on success, 0 on error.
 */
size_t eth_send(session_t *session, const uint8_t dst_addr[],
                const uint8_t data[], size_t data_len);

/**
 * Receives data from the hardware layer, through the ethernet layer.
 * @param session The session object created by net_init().
 * @param data The buffer to which the data will be written. Its length should
 * be at least ETH_DATA_MAX_LEN.
 * @returns Number of bytes written into data array, 0 on error.
 */
size_t eth_recv(session_t *session, uint8_t data[]);


#endif
