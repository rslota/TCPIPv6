#include "ip.h"
#include "session.h"

#include <arpa/inet.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

void send_udp_dummy(session_t *session, const uint8_t dst_ip[])
{
    typedef union PACKED udp_datagram
    {
        struct PACKED
        {
            uint16_t src_port;
            uint16_t dst_port;
            uint16_t datagram_len;
            uint16_t checksum;
            uint8_t data[IP_DATA_MAX_LEN - 8];
        };

        uint8_t buffer[IP_DATA_MAX_LEN];

    } udp_datagram_t;

    udp_datagram_t datagram;

    uint16_t datagram_len = 8 + strlen("Hello world!") + 1;
    strcpy((char*) datagram.data, "Hello world!");

    datagram.src_port = htons(2048);
    datagram.dst_port = htons(55096);
    datagram.datagram_len = htons(datagram_len);
    datagram.checksum = 0;

    datagram.checksum = ip_chksum(session, dst_ip, IP_PROTOCOL_UDP,
                                  datagram.buffer, datagram_len);


    ip_send(session, dst_ip, IP_PROTOCOL_UDP, datagram.buffer, datagram_len);
}

int main(void) {
    char ifname[1024] = "lo";
    uint8_t dst_ip[] = { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
    uint8_t src_ip[] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

    session_t *session = session_open(ifname, src_ip);
    send_udp_dummy(session, dst_ip);
    session_close(session);

    // printf("Options:\n");
    // printf("\tbind  <ifname>\t- bind to the interface ifname\n");
    // printf("\tsrcip <ipv6>  \t- set the source ip (format: aa::aa::aa::...)\n");
    // printf("\tdstip <ipv6>  \t- set the destination ip (format: aa::aa::..)\n");
    // printf("\tsend <text>   \t- send a packet containing text\n");
    // printf("\trecv          \t- wait for a packet and print out its data\n");

    // while(true)
    // {
    //     printf("> ");

    //     char buffer[1024];
    //     scanf("%s", buffer);
    //     if(strcmp(buffer, "bind") == 0)
    //     {
    //         scanf("%s", ifname);
    //     }
    //     else if(strcmp(buffer, "srcip") == 0)
    //     {
    //         scanf("%s", buffer);
    //         inet_pton(AF_INET6, buffer, src_ip);
    //     }
    //     else if(strcmp(buffer, "dstip") == 0)
    //     {
    //         scanf("%s", buffer);
    //         inet_pton(AF_INET6, buffer, src_ip);
    //     }
    //     else if(strcmp(buffer, "send") == 0)
    //     {
    //         // Get line
    //         fgets(buffer, sizeof(buffer), stdin);
    //         ungetc('\n', stdin);

    //         session_t *session = session_open(ifname, src_ip);
    //         ip_send(session, dst_ip, (uint8_t*) buffer, strlen(buffer));
    //         session_close(session);
    //     }
    //     else if(strcmp(buffer, "recv") == 0)
    //     {
    //         session_t *session = session_open(ifname, src_ip);
    //         ip_recv(session, (uint8_t*) buffer);
    //         session_close(session);

    //         printf("Received data: %s", buffer);
    //     }

    //     // Flush stdin
    //     scanf("%*[^\n]\n");
    // }

    return 0;
}
