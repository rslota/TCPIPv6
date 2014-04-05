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
