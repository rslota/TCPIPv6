//
//  eth_common.h
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_eth_common_h
#define TCP_IP_Stack_eth_common_h

#define MAC_ADDR_SIZE       6
#define ETH_MAX_FRAME_SIZE  1500

struct eth_frame
{
    char    preamble;
    char    dest_mac[MAC_ADDR_SIZE];
    char    src_mac[MAC_ADDR_SIZE];
    char    ether_type[2];
    char    payload[ETH_MAX_FRAME_SIZE];
    int     check_seq   : 32;
    char    idle[12];
};

typedef struct eth_frame eth_frame_t;

#endif
