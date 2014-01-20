#ifndef TCPIPStack_external_h
#define TCPIPStack_external_h


#include <stddef.h>
#include <stdint.h>

/**
 * Initialize data needed for hardware-level communication.
 * @returns a session id associated with the session, -1 on error.
 */
int hw_init(const char interface[]);

/**
 * Retrieve the hardware address of an interface identified by param interface.
 * @param interface the interface name.
 * @param addr the buffer into which the address will be written.
 * @returns 0 on success, -1 on error.
 */
int hw_if_addr(int session_id, const char interface[], uint8_t addr[]);

int ip_if_addr(int session_id, const char interface[], uint8_t addr[]);

/**
 * Frees any resources allocated by hw_session_open().
 * @returns 0 on success, -1 on error.
 */
int hw_free(int session_id);

size_t hw_send(int session_id, uint8_t data[], size_t data_len);

/**
 * @note the function should return only when the whole frame was written to
 * the buffer.
 */
size_t hw_recv(int session_id, uint8_t buffer[], size_t buffer_len);

/**
 * Convert value from host to network byte order.
 */
int16_t netb_s(int16_t value);

/**
 * Convert value from network to byte byte order.
 */
int16_t hostb_s(int16_t value);

/**
 * Convert value from network to byte byte order.
 */
int32_t hostb_l(int32_t value);

/**
 * Convert value from host to network byte order.
 */
int32_t netb_l(int32_t value);

/**
 * Convert string IPv6 addr to binary form.
 * @returns 1 on success, 0 on failure
 */
int8_t inet_from_str(const char str[], uint8_t addr[]);

typedef struct thread thread_t;

/**
 * Starts new thread executing given function.
 * @returns thread handle
 */
thread_t* thread_spawn(void* (*func)(void *data), void *data);

#endif
