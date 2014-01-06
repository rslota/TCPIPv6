#ifndef TCPIPStack_icmp_h
#define TCPIPStack_icmp_h

#include "common.h"
#include <memory.h>

#define AARCH_ADDR_LEN 8

#define ICMP_DATA_MAX_LEN 500
#define ICMP_MAX_LEN 500

#define ICMP_TYPE_NEIGHBOR_SOLICITATION     135
#define ICMP_TYPE_NEIGHBOR_ADVERTISEMENT    136

#define NDP_SOURCE_LINK_ADDR_OPT            1
#define NDP_TARGET_LINK_ADDR_OPT            2

// For detailed NDP messages' structure see rfc4861

typedef struct PACKED icmp_packet {
    union PACKED {
        struct PACKED {
            uint8_t         type;
            uint8_t         code;
            uint16_t        checksum;
            uint8_t         body[ICMP_DATA_MAX_LEN];  
        };

        uint8_t buffer[ICMP_MAX_LEN];
    };
} icmp_packet_t;  

// Neighbor Solicitation Message Format
// Neighbor Advertisement Message Format
typedef struct PACKED ndp_neighbor_discover {
    union PACKED {
        struct PACKED {
            uint32_t        reserved;
            uint8_t         target_addr[IP_ADDR_LEN];
            uint8_t         options[ICMP_DATA_MAX_LEN];
        };
        uint8_t buffer[ICMP_MAX_LEN];
    };
} ndp_neighbor_discover_t;

typedef struct PACKED ndp_option {
    union PACKED {
        struct PACKED {
            uint8_t         type;
            uint8_t         len;

            uint8_t         body[ICMP_DATA_MAX_LEN];
        };
        uint8_t buffer[ICMP_MAX_LEN];
    };
} ndp_option_t;


size_t icmp_send(session_t *session, const uint8_t dst_ip[], uint8_t type, uint8_t code, const uint8_t body[], size_t message_len);

size_t icmp_recv(session_t *session, icmp_packet_t *packet);

size_t ndp_solicitate_send(session_t *session, const uint8_t dst_ip[]);

size_t ndp_advertisement_recv(session_t *session, ndp_neighbor_discover_t *ndp);


#endif // TCPIPStack_icmp_h