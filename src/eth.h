/**
 * @file eth.h This file contains functions and definitions connected to the
 *  OSI layer 2 functionality.
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
 * Send data through the ethernet layer, to the hardware layer.
 * @param session The session object created by net_init().
 * @param dst_addr The destination MAC address.
 * @param data The data to send through the ethernet layer.
 * @param data_len The length of data.
 * @returns Number of bytes of data that were sent on success, 0 on error.
 */
size_t eth_send(session_t *session, const uint8_t dst_addr[],
                const uint8_t data[], size_t data_len);

/**
 * Receive data from the hardware layer, through the ethernet layer.
 * @param Data the array to which the data will be written. Its length should
 * be at least ETH_DATA_MAX_LEN.
 * @returns Number of bytes written into data array, 0 on error.
 */
size_t eth_recv(session_t *session, uint8_t data[]);


#endif
