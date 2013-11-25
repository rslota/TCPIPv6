//
//  ip_session.c
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "ip_session.h"
#include "ip_common.h"
#include "ip_assembler.c"
#include <string.h>

int ip_send(int session_id, const char dest_addr[], const char payload[], int payload_size)
{
    if(payload_size > IP_MAX_FRAME_SIZE)
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
        return IP_FRAME_ASSEBLY_ERROR;
    }
    
    int err;
    eth_frame_t eth_frame;
    while((err = IPASM_next_eth(asm_ctx, &eth_frame)) == NEXT_ETH_FRAME)
    {
        /// @todo: send eth_frame via network driver
    }
    
    IPASM_free(asm_ctx);
    
    if(err != 0)
        IP_FRAME_ASSEBLY_ERROR;
    
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
            return IP_FRAME_ASSEBLY_ERROR;
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
