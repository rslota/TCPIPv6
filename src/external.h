#ifndef TCPIPStack_external_h
#define TCPIPStack_external_h


#include <stdlib.h>
#include <stdint.h>

/**
 * Initialize data needed for hardware-level communication.
 * @returns a session id associated with the session, -1 on error.
 */
int hw_session_open(const char interface[]);

/**
 * Retrieve the hardware address of an interface identified by param interface.
 * @param interface the interface name.
 * @param addr the buffer into which the address will be written.
 * @returns 0 on success, -1 on error.
 */
int hw_interface_addr(int session_id, const char interface[], uint8_t addr[]);

/**
 * Frees any resources allocated by hw_session_open().
 * @returns 0 on success, -1 on error.
 */
int hw_session_close(int session_id);

size_t hw_send(int session_id, uint8_t data[], size_t data_len);

/**
 * @note the function should return only when the whole frame was written to
 * the buffer.
 */
size_t hw_recv(int session_id, uint8_t buffer[], size_t buffer_len);

/**
 * Convert value from host to network byte order.
 */
int16_t network_s(int16_t value);

/**
 * Convert value from host to network byte order.
 */
int32_t network_l(int32_t value);

#endif
