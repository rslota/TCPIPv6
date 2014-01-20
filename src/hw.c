#include "hw.h"

#include "common.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>

/* Definitions for use with Linux AF_PACKET sockets.
 Copyright (C) 1998-2013 Free Software Foundation, Inc.
 This file is part of the GNU C Library.
 
 The GNU C Library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 The GNU C Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with the GNU C Library; if not, see
 <http://www.gnu.org/licenses/>.  */

#ifndef __NETPACKET_PACKET_H
#define __NETPACKET_PACKET_H    1

struct sockaddr_ll
{
    unsigned short int sll_family;
    unsigned short int sll_protocol;
    int sll_ifindex;
    unsigned short int sll_hatype;
    unsigned char sll_pkttype;
    unsigned char sll_halen;
    unsigned char sll_addr[8];
};

/* Packet types.  */

#define PACKET_HOST             0               /* To us.  */
#define PACKET_BROADCAST        1               /* To all.  */
#define PACKET_MULTICAST        2               /* To group.  */
#define PACKET_OTHERHOST        3               /* To someone else.  */
#define PACKET_OUTGOING         4               /* Originated by us . */
#define PACKET_LOOPBACK         5
#define PACKET_FASTROUTE        6

/* Packet socket options.  */

#define PACKET_ADD_MEMBERSHIP           1
#define PACKET_DROP_MEMBERSHIP          2
#define PACKET_RECV_OUTPUT              3
#define PACKET_RX_RING                  5
#define PACKET_STATISTICS               6

struct packet_mreq
{
    int mr_ifindex;
    unsigned short int mr_type;
    unsigned short int mr_alen;
    unsigned char mr_address[8];
};

#define PACKET_MR_MULTICAST     0
#define PACKET_MR_PROMISC       1
#define PACKET_MR_ALLMULTI      2

#endif  /* netpacket/packet.h */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory.h>
#include <string.h>

int hw_init(const char interface[])
{
	// We request kernel to pass us only frames with protocol set to IPv6.
    // We could filter out non-IP packets manually as well, so it doesn't really
    // matter.
    const uint16_t ETH_PROTOCOL = htons(ETH_PROTOCOL_IPV6);

    const int sock_desc = socket(AF_PACKET, SOCK_RAW, ETH_PROTOCOL);
    if(sock_desc == -1)
        return -1;

    // Create ifreq object to fetch details about our network interface
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, interface, IFNAMSIZ);

    // Get interface's index
    if(ioctl(sock_desc, SIOCGIFINDEX, &ifreq) == -1)
    {
        hw_free(sock_desc);
        return -1;
    }

    // Prepare the sockaddr_ll struct for binding
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = ETH_PROTOCOL;
    addr.sll_ifindex = ifreq.ifr_ifindex;

    // Bind the socket to the interface
    if(bind(sock_desc, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    {
    	hw_free(sock_desc);
    	return -1;
    }

    return sock_desc;
}

int hw_if_addr(int session_id, const char interface[], uint8_t addr[])
{
    // Create ifreq object to fetch details about our network interface
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, interface, IFNAMSIZ);

    // Get interface's HW addr
    if(ioctl(session_id, SIOCGIFHWADDR, &ifreq) == -1)
        return -1;

    memcpy(addr, ifreq.ifr_hwaddr.sa_data, ETH_ADDR_LEN);
    return 0;
}

int ip_if_addr(int session_id, const char interface[], uint8_t addr[])
{
    struct ifaddrs *ifa = NULL,*ifEntry = NULL;
    void *addPtr = NULL;
    int rc = 0;
    char addressBuffer[INET6_ADDRSTRLEN];

    rc = getifaddrs(&ifa);
    if(rc != 0) {
        return -1;
    }
    for(ifEntry=ifa; ifEntry != NULL; ifEntry = ifEntry->ifa_next) {
        if(ifEntry->ifa_addr->sa_data == NULL) {
                continue;
        }

        if(strcmp(ifEntry->ifa_name, interface) != 0) {
            continue;
        }

        if(ifEntry->ifa_addr->sa_family==AF_INET6) {
                 addPtr = &((struct sockaddr_in6 *)ifEntry->ifa_addr)->sin6_addr;
        }
    }

    freeifaddrs(ifa);

    if(!addPtr)
        return -1;

    memcpy(addr, addPtr, IP_ADDR_LEN);

    return 0;
}


int hw_free(int session_id)
{
	return close(session_id);
}

size_t hw_send(int session_id, uint8_t data[], size_t data_len)
{
	return send(session_id, data, data_len, 0);
}

size_t hw_recv(int session_id, uint8_t buffer[], size_t buffer_len)
{
	return recv(session_id, buffer, buffer_len, 0);
}

int16_t netb_s(int16_t value)
{
	return htons(value);
}

int32_t netb_l(int32_t value)
{
	return htonl(value);
}

int16_t hostb_s(int16_t value)
{
    return ntohs(value);
}

int32_t hostb_l(int32_t value)
{
    return ntohl(value);
}

int8_t inet_from_str(const char str[], uint8_t addr[])
{
    return inet_pton(AF_INET6, str, addr);
}

/* Threading */

struct thread {
    pthread_t thread;
};

thread_t* thread_spawn(void*(*func)(void *data), void *data)
{
    thread_t* t = malloc(sizeof(struct thread));

    pthread_create( &t->thread, NULL, func, data);

    return t;
}
