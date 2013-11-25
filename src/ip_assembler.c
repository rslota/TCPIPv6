//
//  ip_assembler.c
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "ip_assembler.h"
#include <stdlib.h>


IPASM *IPASM_new()
{
    IPASM *st = malloc(sizeof(IPASM));
    // TODO: initialization of IPASM
    return st;  
}

int IPASM_assembly(IPASM *ctx, ip_frame_t *frame)
{
    return 0;
}

int IPASM_consume_eth(IPASM *ctx, const eth_frame_t *frame)
{
    return 0;
}

int IPASM_deassembly(IPASM *ctx, const ip_frame_t *frame)
{
    return 0;
}

int IPASM_next_eth(IPASM *ctx, eth_frame_t *frame)
{
    return 0;
}

void IPASM_free(IPASM *ctx)
{
    free(ctx);
}