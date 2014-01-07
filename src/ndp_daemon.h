#ifndef TCPIPStack_ndp_daemon_h
#define TCPIPStack_ndp_daemon_h

#include "common.h"

extern volatile uint8_t is_initialized;

void ndp_initialize(const char *ifname);
void ndp_stop();

void ndp_table_insert(const uint8_t ip_addr[], const uint8_t hw_addr[]);
uint8_t ndp_table_lookup(const uint8_t ip_addr[], uint8_t hw_addr[]);

void ndp_table_print();

#endif // TCPIPStack_ndp_daemon_h