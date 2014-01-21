/**
 * @file ndp_daemon.h This file contains functions responsible for management
 * of the NDP table.
 */

#ifndef TCPIPStack_ndp_daemon_h
#define TCPIPStack_ndp_daemon_h


#include "common.h"

/**
 * Initializes the NDP daemon.
 * @param ifname The interface name on which the daemon will work.
 * @param src_ip_addr The IP address of the daemon.
 */
void ndp_initialize(const char *ifname, const uint8_t src_ip_addr[]);

/**
 * Inserts an entry into the NDP table.
 * @param ip_addr The IP address to insert into the table.
 * @param hw_addr The MAC address to insert into the table.
 */
void ndp_table_insert(const uint8_t ip_addr[], const uint8_t hw_addr[]);

/**
 * Lookups an entry in the NDP table.
 * @param ip_addr The key by which we search in the table.
 * @param hw_addr The buffer in which the looked up value will be stored.
 * @returns 1 when an entry is found, 0 otherwise.
 */
uint8_t ndp_table_lookup(const uint8_t ip_addr[], uint8_t hw_addr[]);

/**
 * A diagnostic function that prints the contents of the NDP table.
 */
void ndp_table_print(void);


#endif // TCPIPStack_ndp_daemon_h
