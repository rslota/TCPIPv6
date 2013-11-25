//
//  ip_assembler.c
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "ip_assembler.h"
#include "session.h"

#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <zlib.h>



static int match_hw_addr(const IPASM *ctx, const char hw_addr[]) // Checks if given hw_addr matches any local interface's mac addr
{
    for(int i = 0; i < ctx->ifs_count; ++i)
        if(!memcmp(ctx->self_hw_addr[i], hw_addr, MAC_ADDR_SIZE))
            return 1;
    
    return 0;
}

static int ip_resolve(const char dest_ip_addr[], char dest_hw_addr[])
{
    // stub
    /// @todo: This method should be moved and implemented
    return 0;
}


IPASM *IPASM_new(int session_id)
{
    struct ifreq *ifr, *ifend;
    struct ifreq ifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];
    
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(session_id, SIOCGIFCONF, &ifc) < 0)
    {
        return 0;
    }
    
    IPASM *st = malloc(sizeof(IPASM));
    
    st->session_id = session_id;
    st->ifs_count = 0;
    
    
    ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));
    for (ifr = ifc.ifc_req; ifr < ifend; ifr++)
    {
        if (ifr->ifr_addr.sa_family == AF_INET6)
        {
            strncpy(ifreq.ifr_name, ifr->ifr_name,sizeof(ifreq.ifr_name));
            if (ioctl (session_id, SIOCGIFHWADDR, &ifreq) < 0)
            {
                free(st);
                return 0;
            }
            
            
            memcpy(st->self_hw_addr[st->ifs_count], &ifreq.ifr_hwaddr.sa_data, 6);
            st->ifs_count++;

            printf("Device %s -> Ethernet %02x:%02x:%02x:%02x:%02x:%02x\n", ifreq.ifr_name,
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[0],
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[1],
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[2],
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[3],
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[4],
                   (int) ((unsigned char *) &ifreq.ifr_hwaddr.sa_data)[5]);
        }
    }
    
    return st;
}

int IPASM_assembly(IPASM *ctx, ip_frame_t *frame)
{
    ctx->empty_ip_frame = frame;
    ctx->ip_frame_offset = 0;
    
    return 0;
}

int IPASM_consume_eth(IPASM *ctx, const eth_frame_t *frame)
{
    if( *((uint16_t*) frame->ether_type) != IPv6_ETHER_TYPE ) // Skip non-inetv6 frames
        return WANT_ETH_FRAME;
    
    if(!match_hw_addr(ctx, frame->src_mac)) // Skip frames that are not addressed to us
        return WANT_ETH_FRAME;
    
    if(ctx->ip_frame_offset == 0 && frame->payload_size < offsetof(struct ip_frame, payload)) // eth frame should have at least 42 bytes, so we should be able to get full header from one eth frame
        return WANT_ETH_FRAME;
    
    size_t copy_size = sizeof(ip_frame_t) - ctx->ip_frame_offset;
    if(frame->payload_size < copy_size)
       copy_size = frame->payload_size;
       
    memcpy(ctx->empty_ip_frame + ctx->ip_frame_offset, frame->payload, copy_size);
    ctx->ip_frame_offset += copy_size;
       
    if(ctx->empty_ip_frame->next_header != TCP_PROTOCOL || ctx->empty_ip_frame->next_header != UDP_PROTOCOL)
       return UNSUPPORTED_IP_FRAME;
       
    size_t current_payload_size = ctx->ip_frame_offset - offsetof(struct ip_frame, payload);
    if(current_payload_size < ctx->empty_ip_frame->payload_size)
       return WANT_ETH_FRAME;
    
    return 0;
}

int IPASM_deassembly(IPASM *ctx, const ip_frame_t *frame)
{
    ctx->ip_frame_offset = 0;
    ctx->src_ip_frame = frame;
    if(ip_resolve(frame->dest_addr, ctx->dest_hw_addr) != 0)
        return DEST_ADDR_RESOLVE_ERROR;
    
    return 0;
}

int IPASM_next_eth(IPASM *ctx, eth_frame_t *frame)
{
    // Handle ETH header
    memset(frame->src_mac, 0, MAC_ADDR_SIZE); // Just let driver handle it
    memcpy(frame->dest_mac, ctx->dest_hw_addr, MAC_ADDR_SIZE);
    
    frame->preamble = 1 << 7 | 1 << 5 | 1 << 3 | 1 << 1 | 1; // 10101011b, however I'm still not sure if we need to send this field
    *((uint16_t*)frame->ether_type) = IPv6_ETHER_TYPE;

    frame->payload_size = 0;
    
    // Handle IP header
    if(ctx->ip_frame_offset == 0) // Headers not set
    {
        memcpy(frame->payload, ctx->src_ip_frame, offsetof(struct ip_frame, payload));
        ctx->ip_frame_offset += offsetof(struct ip_frame, payload);
        frame->payload_size += offsetof(struct ip_frame, payload);
    }
    
    // Handle payload
                       // payload full size               payload copied till now
    size_t copy_size = ctx->src_ip_frame->payload_size - (ctx->ip_frame_offset - offsetof(struct ip_frame, payload)); // IP payload left
    if(ETH_MAX_FRAME_SIZE - frame->payload_size < copy_size)
        copy_size = ETH_MAX_FRAME_SIZE - frame->payload_size;
    
    memcpy(frame->payload + frame->payload_size, ctx->src_ip_frame + ctx->ip_frame_offset, copy_size);
    ctx->ip_frame_offset += copy_size;
    frame->payload_size += copy_size;
    
    /// @todo: For now ignore the fact that eth frame's payload shall have at least 42B
    
    // ETH CRC
    frame->check_seq = crc32(0, (const Bytef*)frame, (uInt)(offsetof(struct eth_frame, payload) + frame->payload_size));
    
    // Next eth frame will be generated
    if(ctx->ip_frame_offset < offsetof(struct ip_frame, payload) + ctx->src_ip_frame->payload_size)
        return NEXT_ETH_FRAME;
    
    return 0;
}

void IPASM_free(IPASM *ctx)
{
    free(ctx);
}
