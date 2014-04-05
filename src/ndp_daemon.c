/**
 * @copyright Copyright © 2013-2014, Rafał Słota, Konrad Zemek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ndp_daemon.h"

#include "hw.h"
#include "icmp.h"
#include "net.h"

#include <memory.h>
#include <stdlib.h>
#include <time.h>

#define NDP_TABLE_MAX_SIZE 100000

typedef struct ndp_record
{
    uint8_t ip_addr[IP_ADDR_LEN];
    uint8_t hw_addr[ETH_ADDR_LEN];

    time_t valid_to;
} ndp_record_t;

volatile uint8_t is_initialized = 0;

static thread_t *receiver_thread, *sender_thread;
static uint8_t src_ip_addr[IP_ADDR_LEN];
static char ifname[1024];

// @todo synchronization
static ndp_record_t*  ndp_table [ NDP_TABLE_MAX_SIZE ];

static void *recv_loop(void *data)
{
    session_t *session = net_init(ifname, src_ip_addr, 0, 0, 0, ICMP, -1);
    icmp_packet_t packet;
    ndp_neighbor_discover_t n_discvr;

    while(is_initialized)
    {
        size_t recv = icmp_recv(session, &packet);
        if(!recv)
            continue;

        switch(packet.type)
        {
            case ICMP_TYPE_NEIGHBOR_SOLICITATION:
                memcpy(n_discvr.buffer, packet.body, recv);

                if(memcmp(n_discvr.target_addr, session->src_ip,
                          IP_ADDR_LEN) != 0)
                    continue;

                ndp_option_t opt;
                memcpy(opt.buffer, n_discvr.options, recv + NDP_ND_HEADER_LEN);

                if(opt.type != NDP_SOURCE_LINK_ADDR_OPT || opt.len != 1)
                {
                    fprintf(stderr, "Error: Invalid NDP option type: %d. "
                                    "Expected: %d (NDP_SOURCE_LINK_ADDR_OPT)"
                                    "\n.", opt.type, NDP_SOURCE_LINK_ADDR_OPT);
                    continue;
                }

                ndp_table_insert(session->last_sender_ip, opt.body);

                ndp_neighbor_discover_t ndp_resp;
                ndp_option_t opt_resp;

                memcpy(ndp_resp.target_addr, n_discvr.target_addr, IP_ADDR_LEN);
                // sol & override flag
                ndp_resp.reserved = netb_l( 1 << 30 | 1 << 29 );

                opt_resp.type = NDP_TARGET_LINK_ADDR_OPT;
                opt_resp.len = 1;
                memcpy(opt_resp.body, session->src_addr, ETH_ADDR_LEN);

                size_t opts_len = NDP_OPT_HEADER_LEN + ETH_ADDR_LEN;
                memcpy(ndp_resp.options, opt_resp.buffer, opts_len);

                size_t resp = icmp_send(session, session->last_sender_ip,
                                        ICMP_TYPE_NEIGHBOR_ADVERTISEMENT, 0,
                                        ndp_resp.buffer,
                                        NDP_ND_HEADER_LEN + opts_len);

                if(resp <= 0)
                {
                    fprintf(stderr, "Error: Cannot respond with "
                                    "ICMP_TYPE_NEIGHBOR_ADVERTISEMENT!\n.");
                    continue;
                }

                break;

            case ICMP_TYPE_NEIGHBOR_ADVERTISEMENT:
                memcpy(n_discvr.buffer, packet.body, recv);
                uint8_t zeros[IP_ADDR_LEN] = { 0x0 };

                if(memcmp(zeros, n_discvr.target_addr, IP_ADDR_LEN) == 0)
                    continue; // not sure why it's even possible...

                if(recv <= offsetof(ndp_neighbor_discover_t, options))
                    continue; // If there's no option, ignore

                memcpy(opt.buffer, n_discvr.options, recv + NDP_ND_HEADER_LEN);

                if(opt.type != NDP_TARGET_LINK_ADDR_OPT || opt.len != 1)
                {
                    fprintf(stderr, "Error: Invalid NDP option type: %d. "
                                    "Expected: %d (NDP_TARGET_LINK_ADDR_OPT)"
                                    "\n.", opt.type, NDP_TARGET_LINK_ADDR_OPT);
                    continue;
                }

                ndp_table_insert(n_discvr.target_addr, opt.body);

                break;

            default:
                break; // unsupported ICMP type
        }
    }

    net_free(session);
    return 0;
}

static void* send_loop(void *data)
{

    while(0 && is_initialized)
    {

    }

    return 0;
}

void ndp_table_insert(const uint8_t ip_addr[], const uint8_t hw_addr[])
{
    uint32_t first_free = NDP_TABLE_MAX_SIZE + 1;

    ndp_record_t *rec = malloc(sizeof(ndp_record_t));
    memcpy(rec->ip_addr, ip_addr, IP_ADDR_LEN);
    memcpy(rec->hw_addr, hw_addr, ETH_ADDR_LEN);
    rec->valid_to = time(NULL) + 60 * 5; // 5 mins

    for(int i = 0; i < NDP_TABLE_MAX_SIZE; ++i)
    {
        if(ndp_table[i] && time(NULL) > ndp_table[i]->valid_to)
        {
            free(ndp_table[i]);
            ndp_table[i] = 0;
        }

        if(!ndp_table[i] && i < first_free)
            first_free = i;

        if(ndp_table[i])
        {
            if(memcmp(ndp_table[i]->ip_addr, rec->ip_addr, IP_ADDR_LEN) == 0)
            {
                free(ndp_table[i]);
                ndp_table[i] = rec;

                return;
            }
        }
    }

    if(first_free < NDP_TABLE_MAX_SIZE)
    {
        ndp_table[first_free] = rec;
    }
    else
    {
        first_free = rand() % NDP_TABLE_MAX_SIZE;
        free(ndp_table[ first_free ]);
        ndp_table[ first_free ] = rec;
    }
}

uint8_t ndp_table_lookup(const uint8_t ip_addr[], uint8_t hw_addr[])
{
    for(int i = 0; i < NDP_TABLE_MAX_SIZE; ++i)
    {
        if(ndp_table[i] && time(NULL) > ndp_table[i]->valid_to)
        {
            free(ndp_table[i]);
            ndp_table[i] = 0;
        }

        if(ndp_table[i] &&
           memcmp(ndp_table[i]->ip_addr, ip_addr, IP_ADDR_LEN) == 0)
        {
            memcpy(hw_addr, ndp_table[i]->hw_addr, ETH_ADDR_LEN);

            return 1;
        }
    }

    return 0;
}

static void ndp_stop(void)
{
    is_initialized = 0;
    free(receiver_thread);
    free(sender_thread);
}

void ndp_initialize(const char *_ifname, const uint8_t _src_ip_addr[])
{
    if(is_initialized)
        return;

    is_initialized = 1;
    strcpy(ifname, _ifname);
    memcpy(src_ip_addr, _src_ip_addr, IP_ADDR_LEN);

    for(int i = 0; i < NDP_TABLE_MAX_SIZE; ++i) {
        ndp_table[i] = 0;
    }

    receiver_thread = thread_spawn(&recv_loop, 0);
    sender_thread = thread_spawn(send_loop, 0);

    atexit(ndp_stop);
}

void ndp_table_print(void)
{
    printf("***********************\n");
    printf(">>>>>> NDP Table <<<<<<\n");
    printf("***********************\n");
    for(int i = 0; i < NDP_TABLE_MAX_SIZE; ++i)
    {
        if(ndp_table[i] && time(NULL) > ndp_table[i]->valid_to)
        {
            free(ndp_table[i]);
            ndp_table[i] = 0;
        }

        if(ndp_table[i])
        {
            uint16_t *ip = (uint16_t*)ndp_table[i]->ip_addr;
            uint8_t *hw = ndp_table[i]->hw_addr;
            printf("\t(Index: %d, Valid: %lds): %04hx::%04hx::%04hx::%04hx::"
                   "%04hx::%04hx::%04hx::%04hx ----->>>> %02hhx:%02hhx:%02hhx:"
                   "%02hhx:%02hhx:%02hhx\n", i,
                   ndp_table[i]->valid_to - time(NULL), hostb_s(ip[0]),
                   hostb_s(ip[1]), hostb_s(ip[2]), hostb_s(ip[3]),
                   hostb_s(ip[4]), hostb_s(ip[5]), hostb_s(ip[6]),
                   hostb_s(ip[7]), hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
        }
    }
}
