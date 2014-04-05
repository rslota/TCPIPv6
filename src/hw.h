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
 * @file hw.h This file contains functions and definitions connected to the
 * OSI layer 1 functionality. The functions from this file need to be redefined
 * in order to port the stack to a different platform. The reference
 * implementation supplied in hw.c works on Linux 2.6+ kernels.
 */

#ifndef TCPIPStack_hw_h
#define TCPIPStack_hw_h


#include <stddef.h>
#include <stdint.h>

/**
 * Initialize data needed for hardware-level communication.
 * @returns A session id associated with the hardware session on success, -1 on
 * error.
 */
int hw_init(const char interface[]);

/**
 * Retrieve the hardware address of an interface identified by param interface.
 * @param session_id The id number returned by hw_init().
 * @param interface The name of an interface.
 * @param addr The buffer into which the address will be written.
 * @returns 0 on success, -1 on error.
 */
int hw_if_addr(int session_id, const char interface[], uint8_t addr[]);

/**
 * Frees any resources allocated by hw_init().
 * @param session_id The id number returned by hw_init().
 * @returns 0 on success, -1 on error.
 */
int hw_free(int session_id);

/**
 * Sends the link layer frame through the network.
 * @param session_id The id number returned by hw_init().
 * @param data The link layer frame to be sent.
 * @param data_len The length of data.
 * @returns The number of bytes of data sent on success, 0 on error.
 * @note The link layer frame passed to hw_send consists of MAC destination,
 * MAC source, 802.1Q tag, ethertype and payload fields. The preamble, frame
 * delimiter, frame check sequence and interframe gap have to be added by
 * hw_send() implementation.
 */
size_t hw_send(int session_id, uint8_t data[], size_t data_len);

/**
 * Receives data from the network.
 * @param session_id The id number returned by hw_init().
 * @param timeout The timeout in milliseconds for the receive operation; -1
 * means infinity.
 * @param buffer The buffer into which the data contained in the link layer
 * frame will be written.
 * @param buffer_len The length of the buffer.
 * @returns The number of bytes of data written into buffer on success, 0 on
 * error.
 * @note This function should block until the whole frame was received or the
 * timeout has occurred.
 */
size_t hw_recv(int session_id, int timeout, uint8_t buffer[],
               size_t buffer_len);

/**
 * Converts a 2-byte value from host to network byte order.
 * @param value The value to be converted to network byte order.
 * @returns The value in network byte order.
 */
uint16_t netb_s(uint16_t value);

/**
 * Converts a 2-byte value from network to host byte order.
 * @param value The value to be converted from network byte order.
 * @returns The value in host byte order.
 */
uint16_t hostb_s(uint16_t value);

/**
 * Converts a 4-byte value from network to host byte order.
 * @param value The value to be converted from network byte order.
 * @returns The value in host byte order.
 */
uint32_t hostb_l(uint32_t value);

/**
 * Converts a 4-byte value from host to network byte order.
 * @param value The value to be converted to network byte order.
 * @returns The value in network byte order.
 */
uint32_t netb_l(uint32_t value);

typedef struct thread thread_t;

/**
 * Starts new thread executing given function.
 * @param func The function to be executed in a new thread.
 * @param data The data to be supplied as parameter of the executed function.
 * @returns A handle of the created thread.
 */
thread_t* thread_spawn(void* (*func)(void*), void *data);


#endif
