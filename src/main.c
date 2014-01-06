#include "ip.h"
#include "net.h"
#include "hw.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

int main(void) {
    char ifname[1024] = "lo";
    uint8_t dst_ip[] = { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
    uint8_t src_ip[] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

    printf("Options:\n");
    // printf("\tbind  <ifname>\t- bind to the interface ifname\n");
    // printf("\tsrcip <ipv6>  \t- set the source ip (format: aa::aa::aa::...)\n");
    // printf("\tdstip <ipv6>  \t- set the destination ip (format: aa::aa::..)\n");
    printf("\tudp   <text>  \t- send a packet containing text\n");
    printf("\trecv          \t- wait for a packet and print out its data\n");

    while(true)
    {
        printf("> ");

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
            fgets(buffer, sizeof(buffer), stdin);
            ungetc('\n', stdin);

            session_t *session = net_init(ifname, src_ip, 2048, UDP);
            net_send(session, dst_ip, 55300, (uint8_t*) buffer, strlen(buffer));
            net_free(session);
        }
        else if(strcmp(buffer, "recv") == 0)
        {
            session_t *session = net_init(ifname, src_ip, 55300, UDP);
            net_recv(session, (uint8_t*) buffer, sizeof(buffer));
            net_free(session);

            printf("Received data: %s", buffer);
        }

        // Flush stdin
        scanf("%*[^\n]\n");
    }

    return 0;
}
