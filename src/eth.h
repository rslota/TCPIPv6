#ifndef TCPIPStack_ethernet_h
#define TCPIPStack_ethernet_h


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
 * @returns number of bytes of data that were sent on success, 0 on error.
 */
size_t eth_send(session_t *session, const uint8_t dst_addr[],
                const uint8_t data[], size_t data_len);

/**
 * Receive data from the hardware layer, through the ethernet layer.
 * @param data The array to which the data will be written. Its length should
 * be at least ETH_DATA_MAX_LEN.
 * @returns number of bytes written into data array, 0 on error.
 */
size_t eth_recv(session_t *session, uint8_t data[]);


#endif
