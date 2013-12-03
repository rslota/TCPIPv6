//
//  ip_session.c
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "ip_session.h"
#include "ip_common.h"
#include "ip_assembler.c"

#include <linux/if_packet.h>

#include <string.h>
#include <stdbool.h>

int ip_send(int session_id, const char dest_addr[], const char payload[], int payload_size)
{
    if(payload_size > IP_MAX_DATA_SIZE)
        return IP_PAYLOAD_TO_BIG;

    ip_frame_t frame;
    memcpy(frame.dest_addr, dest_addr, IP_ADDR_SIZE);
    /// @todo: src_addr
    frame.flow_label = session_id & 0xfffff; // It could speed up TCP, but routers can just ignore it, so 0 value isnt so bad either
    frame.hop_limit = IP_PACKET_LIFETIME;
    frame.version = 6;
    frame.traffic_class = 0; /// @todo: find out what is it exactly. Right now can be ignored
    frame.payload_size = payload_size;

    memcpy(frame.payload, payload, payload_size);

    IPASM *asm_ctx = IPASM_new(session_id);

    if(IPASM_deassembly(asm_ctx, &frame))
    {
        IPASM_free(asm_ctx);
        return IP_FRAME_ASSEMBLY_ERROR;
    }

    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifindex;
    addr.sll_halen = ETH_ALEN;
    memcpy(addr.sll_addr, asm_ctx->dest_hw_addr, ETH_ALEN);

    int err;
    eth_frame_t eth_frame;
    while(true)
    {
        err = IPASM_next_eth(asm_ctx, &eth_frame);
        if(err == 0 || err == NEXT_ETH_FRAME)
            if(sendto(session_id, eth_frame.buffer, sizeof(eth_frame.buffer), 0, (struct sockaddr*) &addr, sizeof(addr)) < 0)
                return NETWORK_SEND_ERROR;

        if(err != NEXT_ETH_FRAME)
            break;
    }

    IPASM_free(asm_ctx);

    if(err != 0)
        IP_FRAME_ASSEMBLY_ERROR;

    return 0;
}

int ip_recv(int session_id, const char src_addr[], char buffer[], int buffer_max_size) /// @todo: add timeout for this blocking call?
{
    int err;
    ip_frame_t ip_frame;
    IPASM *asm_ctx = IPASM_new(session_id);

    while(1)
    {
        if(IPASM_assembly(asm_ctx, &ip_frame))
        {
            IPASM_free(asm_ctx);
            return IP_FRAME_ASSEMBLY_ERROR;
        }

        eth_frame_t eth_frame; // Build IP packet
        do
        {
            /// @todo receive eth_frame from network driver
            err = IPASM_consume_eth(asm_ctx, &eth_frame);
        } while (err == WANT_ETH_FRAME);

        if(err == 0 && memcmp(src_addr, ip_frame.src_addr, IP_ADDR_SIZE) == 0)
            break;
    }

    IPASM_free(asm_ctx);

    return 0;
}
