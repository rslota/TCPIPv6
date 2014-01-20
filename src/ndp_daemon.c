#include "ndp_daemon.h"
#include "hw.h"
#include "icmp.h"
#include "net.h"

#include <time.h>
#include <stdlib.h>

#define NDP_TABLE_MAX_SIZE 100000

typedef struct ndp_record {
    uint8_t ip_addr[IP_ADDR_LEN];
    uint8_t hw_addr[ETH_ADDR_LEN];

    time_t valid_to;
} ndp_record_t;

volatile uint8_t is_initialized = 0;

static thread_t *receiver_thread, *sender_thread;
static char ifname[1024];

// @todo synchronization
static ndp_record_t*  ndp_table [ NDP_TABLE_MAX_SIZE ];

void* recv_loop(void *data)
{
    session_t *session = net_init(ifname, 0, ICMP);
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
                //fprintf(stdout, "Debug: Handling ICMP_TYPE_NEIGHBOR_SOLICITATION with target addr: %04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx\n",
                //    ((uint16_t*)n_discvr.target_addr)[0], ((uint16_t*)n_discvr.target_addr)[1], ((uint16_t*)n_discvr.target_addr)[2], ((uint16_t*)n_discvr.target_addr)[3], ((uint16_t*)n_discvr.target_addr)[4], ((uint16_t*)n_discvr.target_addr)[5], ((uint16_t*)n_discvr.target_addr)[6], ((uint16_t*)n_discvr.target_addr)[7]);
                
                if(memcmp(n_discvr.target_addr, session->src_ip, IP_ADDR_LEN) != 0) 
                {
                    //fprintf(stdout, "Debug: Skipping ICMP_TYPE_NEIGHBOR_SOLICITATION, invalid target_addr.");
                    continue;
                }

                ndp_option_t opt;
                memcpy(opt.buffer, n_discvr.options, recv + offsetof(ndp_neighbor_discover_t, options));

                if(opt.type != NDP_SOURCE_LINK_ADDR_OPT || opt.len != 1) 
                {
                    fprintf(stdout, "Error: Invalid NDP option type: %d. Expected: %d (NDP_SOURCE_LINK_ADDR_OPT)\n.", opt.type, NDP_SOURCE_LINK_ADDR_OPT);
                    continue;
                }

                ndp_table_insert(session->last_sender_ip, opt.body);

                ndp_neighbor_discover_t ndp_resp;
                ndp_option_t opt_resp;

                memcpy(ndp_resp.target_addr, n_discvr.target_addr, IP_ADDR_LEN);
                ndp_resp.reserved = netb_l( 1 << 30 | 1 << 29 ); // sol & override flag

                opt_resp.type = NDP_TARGET_LINK_ADDR_OPT;
                opt_resp.len = 1;
                memcpy(opt_resp.body, session->src_addr, ETH_ADDR_LEN);

                size_t opts_len = offsetof(ndp_option_t, body) + ETH_ADDR_LEN;
                memcpy(ndp_resp.options, opt_resp.buffer, opts_len);

                size_t resp = icmp_send(session, session->last_sender_ip, ICMP_TYPE_NEIGHBOR_ADVERTISEMENT, 0, ndp_resp.buffer, offsetof(ndp_neighbor_discover_t, options) + opts_len);
                if(resp <= 0)
                {
                    fprintf(stdout, "Error: Cannot respond with  ICMP_TYPE_NEIGHBOR_ADVERTISEMENT!\n.");
                    continue;
                }

                break;
            case ICMP_TYPE_NEIGHBOR_ADVERTISEMENT:
                //fprintf(stdout, "Debug: Handling ICMP_TYPE_NEIGHBOR_ADVERTISEMENT with target addr: %04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx\n",
                //    ((uint16_t*)n_discvr.target_addr)[0], ((uint16_t*)n_discvr.target_addr)[1], ((uint16_t*)n_discvr.target_addr)[2], ((uint16_t*)n_discvr.target_addr)[3], ((uint16_t*)n_discvr.target_addr)[4], ((uint16_t*)n_discvr.target_addr)[5], ((uint16_t*)n_discvr.target_addr)[6], ((uint16_t*)n_discvr.target_addr)[7]);
                
                memcpy(n_discvr.buffer, packet.body, recv);

                uint8_t zeros[IP_ADDR_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
                if(memcmp(zeros, n_discvr.target_addr, IP_ADDR_LEN) == 0)
                {
                    // not sure why it's even possible... 
                    continue;
                }

                if(recv <= offsetof(ndp_neighbor_discover_t, options)) 
                {
                    // If there's no option, ignore 
                    continue;
                }

                memcpy(opt.buffer, n_discvr.options, recv + offsetof(ndp_neighbor_discover_t, options));

                if(opt.type != NDP_TARGET_LINK_ADDR_OPT || opt.len != 1) 
                {
                    fprintf(stdout, "Error: Invalid NDP option type: %d. Expected: %d (NDP_TARGET_LINK_ADDR_OPT)\n.", opt.type, NDP_TARGET_LINK_ADDR_OPT);
                    continue;
                }

                ndp_table_insert(n_discvr.target_addr, opt.body);

                break;
            default:
                //fprintf(stdout, "Debug: Skipping unsupported ICMP packet with type: %d.\n", packet.type);
            ;
        }
    }

    net_free(session);
    return 0;
}

void* send_loop(void *data)
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

    if(first_free < NDP_TABLE_MAX_SIZE) {
        ndp_table[first_free] = rec;
    } else {
        first_free = time(NULL) % NDP_TABLE_MAX_SIZE; /// @todo: pls fix me !
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

        if(ndp_table[i] && memcmp(ndp_table[i]->ip_addr, ip_addr, IP_ADDR_LEN) == 0)
        {
            memcpy(hw_addr, ndp_table[i]->hw_addr, ETH_ADDR_LEN);

            return 1;
        }
    }

    return 0;
}

void ndp_initialize(const char *_ifname)
{
    if(is_initialized)
        return;

    is_initialized = 1;
    strcpy(ifname, _ifname);

    for(int i = 0; i < NDP_TABLE_MAX_SIZE; ++i) {
        ndp_table[i] = 0;
    }

    receiver_thread = thread_spawn(&recv_loop, 0);
    sender_thread = thread_spawn(send_loop, 0);
}

void ndp_stop()
{
    is_initialized = 0;
    free(receiver_thread);
    free(sender_thread);
}

void ndp_table_print() 
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
            printf("\t(Index: %d, Valid: %lds): %04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx::%04hx ----->>>> %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", 
                i, ndp_table[i]->valid_to - time(NULL),
                hostb_s(ip[0]), hostb_s(ip[1]), hostb_s(ip[2]), hostb_s(ip[3]), hostb_s(ip[4]), hostb_s(ip[5]), hostb_s(ip[6]), hostb_s(ip[7]),
                hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
        }
    }
}

