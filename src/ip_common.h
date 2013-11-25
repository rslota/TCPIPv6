//
//  ip.h
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_ip_h
#define TCP_IP_Stack_ip_h

#include <stdint.h>

// Maximum size of IP frame. "Any data link layer conveying IPv6 data must be capable of delivering
// an IP packet containing 1280 bytes without the need to invoke end-to-end fragmentation at the IP layer."
// So if we don't want to implement "Packet too Big" ICMPv6 control messages, we need to limit IP frame size to 1280B.
// Also we should ignore "Fragmentation extension header" - using it in end-to-end fragmenation is pointless.

#define IP_MAX_FRAME_SIZE   1280

#define IP_ADDR_SIZE    16


struct ip_frame
{
    // IP header
    int     version             : 4;
    int     traffic_class       : 8;
    int     flow_label          : 20;
    int     payload_size        : 16;
    int     next_header         : 8;
    int     hop_limit           : 8;
    char    src_addr[IP_ADDR_SIZE];
    char    dest_addr[IP_ADDR_SIZE];
    // We do NOT support extension headers
    
    // IP payload
    char    payload[IP_MAX_FRAME_SIZE];
};

typedef struct ip_frame ip_frame_t;

#endif
