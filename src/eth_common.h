//
//  eth_common.h
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_eth_common_h
#define TCP_IP_Stack_eth_common_h

#include <linux/if_ether.h>

#include <stdint.h>
#include <stddef.h>

struct eth_frame
{
    union
    {
        struct
        {
            struct ethhdr header;
            char data[ETH_DATA_LEN];
        } __attribute__((packed)) field;

        char buffer[ETH_FRAME_LEN];
    } __attribute__((packed));

    size_t payload_size;
};

typedef struct eth_frame eth_frame_t;

#endif
