#include "session.h"

#include "external.h"

#include <memory.h>
#include <stdlib.h>

session_t *session_open(const char *interface, const uint8_t src_ip[], uint16_t port, protocol_t protocol)
{
    session_t *s = malloc(sizeof(session_t));
    if(s == 0)
        return 0;

    s->session_id = hw_session_open(interface);
    if(s->session_id == -1)
    {
        free(s);
        return 0;
    }

    if(hw_interface_addr(s->session_id, interface, s->src_addr) == -1)
    {
        session_close(s);
        return 0;
    }

    memcpy(s->src_ip, src_ip, IP_ADDR_LEN);
    s->port = port;
    s->protocol = (uint16_t) protocol;

    return s;
}

int session_close(session_t *session)
{
    const int err = hw_session_close(session->session_id);
    free(session);
    return err;
}
