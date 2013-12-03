//
//  session.c
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "session.h"

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int ifindex;

/// @todo: error handling
int session(int protocol, const char *iface)
{
    const int fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IPV6));

    // Find interface index
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, iface, IFNAMSIZ);
    ioctl(fd, SIOCGIFINDEX, &ifreq);
    ifindex = ifreq.ifr_ifindex;

    // // Bind to the interface
    // struct sockaddr_ll addr;
    // memset(&addr, 0, sizeof(addr));
    // addr.sll_protocol = htons(ETH_P_IPV6);
    // addr.sll_ifindex = ifreq.ifr_ifindex;
    // int t = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
    // int k = errno;

    // printf("Binding to device %s (%d)", ifreq.ifr_name, ifreq.ifr_ifindex);

    return fd;
}

int destroy(int session_id)
{
    return close(session_id);
}
