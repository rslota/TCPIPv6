#ifndef TCPIPStack_ethernet_h
#define TCPIPStack_ethernet_h


#include "common.h"
#include "session.h"

#include <stddef.h>
#include <stdint.h>

#define ETH_HEADER_LEN    14
#define ETH_DATA_MIN_LEN  46
#define ETH_DATA_MAX_LEN  1500
#define ETH_FRAME_MIN_LEN (ETH_HEADER_LEN + ETH_DATA_MIN_LEN)
#define ETH_FRAME_MAX_LEN (ETH_HEADER_LEN + ETH_DATA_MAX_LEN)

/**
 * Send data through the ethernet layer, to the hardware layer.
 * @returns number of bytes of data parameter that were sent sent.
 * @note A value less than ETH_DATA_MIN_LEN indicates an error. The errno value
 * will contain the error code.
 */
size_t eth_send(session_t *session, const uint8_t dst_addr[],
                const uint8_t data[], size_t data_len);


#endif
