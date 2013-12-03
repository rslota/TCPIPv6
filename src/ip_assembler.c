//
//  ip_assembler.c
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "eth_common.h"
#include "ip_assembler.h"
#include "ip_common.h"
#include "session.h"

#include <linux/if_ether.h>
#include <arpa/inet.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ip_resolve(const char dest_ip_addr[], char dest_hw_addr[])
{
    // stub
    /// @todo: This method should be moved and implemented
    memcpy(dest_hw_addr, "\255\255\255\255\255\255", ETH_ALEN);
    return 0;
}

IPASM *IPASM_new(int session_id)
{
    IPASM *st = malloc(sizeof(IPASM));

    st->session_id = session_id;
    st->ifs_count = 0;

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
    if(ntohs(frame->field.header.h_proto) == ETH_P_IPV6) // Skip non-inetv6 frames
        return WANT_ETH_FRAME;

    // eth frame should have at least 42 bytes, so we should be able to get full header from one eth frame
    if(ctx->ip_frame_offset == 0 && frame->payload_size < offsetof(struct ip_frame, payload))
        return WANT_ETH_FRAME;

    size_t copy_size = sizeof(ip_frame_t) - ctx->ip_frame_offset;
    if(frame->payload_size < copy_size)
       copy_size = frame->payload_size;

    memcpy(ctx->empty_ip_frame + ctx->ip_frame_offset, frame->field.data, copy_size);
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
    memset(frame->field.header.h_source, 0, ETH_ALEN); // Just let driver handle it
    memcpy(frame->field.header.h_dest, ctx->dest_hw_addr, ETH_ALEN);
    frame->field.header.h_proto = htons(ETH_P_IPV6);

    frame->payload_size = 0;

    // Handle IP header
    if(ctx->ip_frame_offset == 0) // Headers not set
    {
        memcpy(frame->field.data, ctx->src_ip_frame, offsetof(struct ip_frame, payload));
        *frame->field.data <<= 4; //hack
        ctx->ip_frame_offset += offsetof(struct ip_frame, payload);
        frame->payload_size += offsetof(struct ip_frame, payload);
    }

    // Handle payload
                       // payload full size               payload copied untill now
    size_t copy_size = ctx->src_ip_frame->payload_size - (ctx->ip_frame_offset - offsetof(struct ip_frame, payload)); // IP payload left
    if(ETH_DATA_LEN - frame->payload_size < copy_size)
        copy_size = ETH_DATA_LEN - frame->payload_size;

    memcpy(frame->field.data + frame->payload_size, (char*) ctx->src_ip_frame + ctx->ip_frame_offset, copy_size);
    ctx->ip_frame_offset += copy_size;
    frame->payload_size += copy_size;

    /// @todo: For now ignore the fact that eth frame's payload shall have at least 42B

    // Next eth frame will be generated
    if(ctx->ip_frame_offset < offsetof(struct ip_frame, payload) + ctx->src_ip_frame->payload_size)
        return NEXT_ETH_FRAME;

    return 0;
}

void IPASM_free(IPASM *ctx)
{
    free(ctx);
}
