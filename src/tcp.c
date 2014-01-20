#include "tcp.h"

size_t tcp_connect(session_t *session, const uint8_t dst_ip[], uint16_t dst_port)
{
    return 0;
}

session_t *tcp_listen(session_t *session, const uint8_t bind_ip[], uint16_t bind_port)
{
    return 0;
}

size_t tcp_send(session_t *session, const uint8_t data[], size_t data_len)
{
    return 0;
}

size_t tcp_recv(session_t *session, uint8_t buffer[], size_t buffer_len)
{
    return 0;
}

size_t tcp_close(session_t *session)
{
    return 0;
}