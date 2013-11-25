//
//  session.c
//  TCP-IP Stack
//
//  Created by Rafał Słota on 25.11.2013.
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#include "session.h"

int session(int protocol)
{
    // return socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); ?
    return -1;
}

int destroy(int session_id)
{
    // return close(session_id); ?
    return -1;
}