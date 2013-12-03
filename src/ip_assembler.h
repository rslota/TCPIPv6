//
//  ip_assembler.h
//  TCP-IP Stack
//
//  Copyright (c) 2013 Rafał Słota, Konrad Zemek. All rights reserved.
//

#ifndef TCP_IP_Stack_ip_assembler_h
#define TCP_IP_Stack_ip_assembler_h


#include <linux/if_ether.h>

// Errors
#define WANT_ETH_FRAME          -1      ///< Some more eth frames are needed to assembly ip frame
#define NEXT_ETH_FRAME          -2      ///< Some more eth frames will be generated from the ip frame
#define UNSUPPORTED_IP_FRAME    -3
#define DEST_ADDR_RESOLVE_ERROR -4

struct ip_frame;
typedef struct ip_frame ip_frame_t;

struct eth_frame;
typedef struct eth_frame eth_frame_t;

/// Assembler's context structure
struct IPASM_st
{
    int     session_id;
    int     ifs_count; // Interfaces count
    union
    {
        ip_frame_t *empty_ip_frame;
        const ip_frame_t *src_ip_frame;
    };
    int ip_frame_offset;
    char dest_hw_addr[ETH_ALEN];

};

typedef struct IPASM_st IPASM;

/**
 * Creates new IPASM structure, initializes it and returns its pointer.
 * Should be freed with IPASM_free/1 later on.
 */
IPASM *IPASM_new(int session_id);

/**
 * Initializes IP frame assembly process (eth frames -> ip frame).
 * @param frame Pointer to empty ip_frame struct which shall be filled with newly
 * assembled ip frame
 * @return 0 on success
 */
int IPASM_assembly(IPASM *ctx, ip_frame_t *frame);

/**
 * Parses given eth_frame and updates current ip frame
 * @param frame Pointer to valid eth_frame struct
 * @return 0 when ip frame has beed successfully constructed. Negative value on error
 * e.g. WANT_ETH_FRAME - next call to this function is expected prior to full ip frame construction.
 */
int IPASM_consume_eth(IPASM *ctx, const eth_frame_t *frame);


/**
 * Initializes IP frame deassembly process (ip frame -> eth frame).
 * @param frame Pointer to valid ip_frame struct which shall be deassembled into eth_frame(s)
 * @return 0 on success
 */
int IPASM_deassembly(IPASM *ctx, const ip_frame_t *frame);

/**
 * Generates next eth_frame from current ip frame
 * @param frame Pointer to empty eth_frame struct which shall be filled with this call
 * @return 0 when whole ip frame has beed successfully deassembled. Negative value on error
 * e.g. NEXT_ETH_FRAME - next call to this function is expected prior to full ip frame deassembly.
 */
int IPASM_next_eth(IPASM *ctx, eth_frame_t *frame);


/**
 * Fully destroys IPASM structure (assembler context)
 */
void IPASM_free(IPASM *ctx);


#endif
