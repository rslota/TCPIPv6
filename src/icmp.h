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

/**
 * @file icmp.h This file contains functions and definitions connected to the
 * ICMP and NDP protocols.
 */

#ifndef TCPIPStack_icmp_h
#define TCPIPStack_icmp_h


#include "common.h"

#define ICMP_HEADER_LEN      4
#define ICMP_MAX_LEN         500
#define ICMP_DATA_MAX_LEN    (ICMP_MAX_LEN - ICMP_HEADER_LEN)
#define NDP_ND_HEADER_LEN    (4 + IP_ADDR_LEN)
#define NDP_ND_MAX_LEN       ICMP_DATA_MAX_LEN
#define NDP_ND_DATA_MAX_LEN  (NDP_ND_MAX_LEN - NDP_ND_HEADER_LEN)
#define NDP_OPT_HEADER_LEN   2
#define NDP_OPT_MAX_LEN      NDP_ND_DATA_MAX_LEN
#define NDP_OPT_DATA_MAX_LEN (NDP_OPT_MAX_LEN - NDP_OPT_HEADER_LEN)
#define ICMP_TYPE_NEIGHBOR_SOLICITATION     135
#define ICMP_TYPE_NEIGHBOR_ADVERTISEMENT    136
#define NDP_SOURCE_LINK_ADDR_OPT            1
#define NDP_TARGET_LINK_ADDR_OPT            2

/// For detailed NDP messages' structure see RFC 4861
typedef union PACKED icmp_packet
{
    struct PACKED
    {
        uint8_t         type;
        uint8_t         code;
        uint16_t        checksum;
        uint8_t         body[ICMP_DATA_MAX_LEN];
    };

    uint8_t buffer[ICMP_MAX_LEN];

} icmp_packet_t;

/**
 * Neighbor Solicitation Message Format
 * Neighbor Advertisement Message Format
 */
typedef union PACKED ndp_neighbor_discover
{
    struct PACKED
    {
        uint32_t        reserved;
        uint8_t         target_addr[IP_ADDR_LEN];
        uint8_t         options[NDP_ND_DATA_MAX_LEN];
    };

    uint8_t buffer[NDP_ND_MAX_LEN];

} ndp_neighbor_discover_t;

typedef union PACKED ndp_option
{
    struct PACKED
    {
        uint8_t         type;
        uint8_t         len;
        uint8_t         body[ICMP_DATA_MAX_LEN + 2];
    };

    uint8_t buffer[ICMP_MAX_LEN];

} ndp_option_t;

/**
 * Sends an ICMP message.
 * @param session The session object created by net_init().
 * @param dst_ip The IP address of ICMP message's recipient.
 * @param type The type of ICMP message (see ICMP header).
 * @param code The code of ICMP message (see ICMP header).
 * @param body The contents of the message.
 * @param body_len The length of body.
 * @returns The number of bytes of body that were sent on success, 0 on error.
 */
size_t icmp_send(session_t *session, const uint8_t dst_ip[], uint8_t type,
                 uint8_t code, const uint8_t body[], size_t body_len);

/**
 * Receives an ICMP message.
 * @param session The session object created by net_init().
 * @param packet The structure that will be populated with ICMP message's data.
 * @returns The length of the body field of the message on success, 0 on error.
 */
size_t icmp_recv(session_t *session, icmp_packet_t *packet);

/**
 * Sends the neighbor solicitation message.
 * @param session The session object created by net_init().
 * @param dst_ip The destination IP address for the message.
 * @returns The number of bytes of body that were sent on success, 0 on error.
 */
size_t ndp_solicitate_send(session_t *session, const uint8_t dst_ip[]);

/**
 * Receives neighbor advertisement message.
 * @param session The session object created by net_init().
 * @param ndp The structure that will be populated with the message's data.
 * @returns The number of bytes written into the ndp structure on success, 0 on
 * error.
 */
size_t ndp_advertisement_recv(session_t *session, ndp_neighbor_discover_t *ndp);


#endif // TCPIPStack_icmp_h
