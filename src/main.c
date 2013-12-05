#include "ethernet.h"
#include "session.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

int main(void) {
    const char *ifname = "eth0";
    const uint8_t dst_addr[] = { 0x33, 0x33, 0x0, 0x0, 0x0, 0x0 };
    const char *data = "Hello world!";
    const size_t data_len = strlen(data);

    session_t *session = session_open(ifname);
    const int ret = eth_send(session, dst_addr, (const uint8_t*)data, data_len);

    // uint8_t buffer[ETH_DATA_MAX_LEN];
    // for(size_t read = 0; (read = eth_recv(session, buffer));)
    //     printf("Received %lu bytes of data\n", read);

    session_close(session);
    return ret;
}
