#include "ip.h"
#include "net.h"
#include "hw.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

int main(void) {
    char ifname[1024] = "eth0";
    uint8_t dst_ip[] = { 0xfe, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xba, 0xe8, 0x56, 0xff, 0xfe, 0x3e, 0x95, 0xc4 }; // fe80::bae8:56ff:fe3e:95c4
    uint8_t src_ip[] = { 0xfe, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x02, 0x1c, 0x42, 0xff, 0xfe, 0x56, 0x2b, 0x2a }; // fe80::21c:42ff:fe56:2b2a

    printf("Options:\n");
    // printf("\tbind  <ifname>\t- bind to the interface ifname\n");
    // printf("\tsrcip <ipv6>  \t- set the source ip (format: aa::aa::aa::...)\n");
    // printf("\tdstip <ipv6>  \t- set the destination ip (format: aa::aa::..)\n");
    printf("\tudp   <text>  \t- send a packet containing text\n");
    printf("\trecv          \t- wait for a packet and print out its data\n");

    while(true)
    {
        printf("> ");

        char addr[1024];
        uint8_t ip_addr[IP_ADDR_LEN];
        int port;
        char buffer[1024];
        scanf("%s", buffer);
        /*if(strcmp(buffer, "bind") == 0)
        {
            scanf("%s", ifname);
        }
        else if(strcmp(buffer, "srcip") == 0)
        {
            scanf("%s", buffer);
            inet_pton(AF_INET6, buffer, src_ip);
        }
        else if(strcmp(buffer, "dstip") == 0)
        {
            scanf("%s", buffer);
            inet_pton(AF_INET6, buffer, src_ip);
        }
        else */if(strcmp(buffer, "udp") == 0)
        {
            // Get line
            if( scanf("%s %d %s", addr, &port, buffer) != 3 ) {
                continue;
            }
            
            if(!inet_from_str(addr, ip_addr)) {
                fprintf(stderr, "Error: Invalid IPv6 addr !");
                continue;
            }

            session_t *session = net_init(ifname, src_ip, port, UDP);
            net_send(session, ip_addr, port, (uint8_t*) buffer, strlen(buffer));
            net_free(session);
        }
        else if(strcmp(buffer, "recv") == 0)
        {
            if( scanf("%d", &port) != 1 ) {
                continue;
            }

            session_t *session = net_init(ifname, src_ip, port, UDP);
            net_recv(session, (uint8_t*) buffer, sizeof(buffer));
            net_free(session);

            printf("Received data: %s", buffer);
        }

        // Flush stdin
        scanf("%*[^\n]\n");
    }

    return 0;
}
