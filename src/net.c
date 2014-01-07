#include "net.h"

#include "hw.h"
#include "ip.h"
#include "udp.h"
#include "ndp_daemon.h"

#include <memory.h>
#include <stdlib.h>

session_t *net_init(const char *interface, uint16_t port, protocol_t protocol)
{
    session_t *s = malloc(sizeof(session_t));
    if(s == 0)
        return 0;

    s->session_id = hw_init(interface);
    if(s->session_id == -1)
    {
        free(s);
        return 0;
    }

    if(hw_if_addr(s->session_id, interface, s->src_addr) == -1)
    {
        net_free(s);
        return 0;
    }

    uint8_t src_ip[IP_ADDR_LEN];
    if(ip_if_addr(s->session_id, interface, s->src_ip) == -1)
    {
        net_free(s);
        return 0;
    }

    s->port = port;

    switch(protocol)
    {
        case TCP:
            s->protocol = IP_PROTOCOL_TCP;
            break;
        case UDP:
            s->protocol = IP_PROTOCOL_UDP;
            break;
        case ICMP:
            s->protocol = IP_PROTOCOL_ICMP;
            break;
        default:
            net_free(s);
            return 0;
    }

    // Save interface name for further use
    strcpy(s->interface, interface);

    ndp_initialize(interface);

    return s;
}

int net_free(session_t *session)
{
    const int err = hw_free(session->session_id);
    free(session);
    return err;
}

size_t net_send(session_t *session, const uint8_t dst_ip[], uint16_t dst_port,
                const uint8_t data[], size_t data_len)
{
    switch(session->protocol)
    {
        case IP_PROTOCOL_TCP:
            return 0;
        case IP_PROTOCOL_UDP:
            return udp_send(session, dst_ip, dst_port, data, data_len);
        default:
            return 0;
    }
}

size_t net_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    switch(session->protocol)
    {
        case IP_PROTOCOL_TCP:
            return 0;
        case IP_PROTOCOL_UDP:
            return udp_recv(session, buffer, buffer_len);
        default:
            return 0;
    }
}
