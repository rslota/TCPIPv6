#include "ip_session.h"

#include "session.h"

#include <string.h>

int main(void)
{
    const char *destination_addr = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    const char *payload = "hello world!";
    const int session_id = session(DEFAULT_PROTOCOL, "eth0");
    while(ip_send(session_id, destination_addr, payload, strlen(payload)) == 0);
    destroy(session_id);
    return 0;
}
