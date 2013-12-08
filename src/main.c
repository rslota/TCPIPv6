#include "ip.h"
#include "session.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

int main(void) {
    const char *ifname = "lo";
    const uint8_t dst_ip[] = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
        0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
    const uint8_t src_ip[] = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
        0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
    const char *data = "Hello world!";
    const size_t data_len = strlen(data);

    session_t *session = session_open(ifname, src_ip);
    const size_t ret = ip_send(session, dst_ip, (const uint8_t*) data, data_len);

    // uint8_t buffer[ETH_DATA_MAX_LEN];
    // for(size_t read = 0; (read = eth_recv(session, buffer));)
    //     printf("Received %lu bytes of data\n", read);

    session_close(session);
    return ret;
}
