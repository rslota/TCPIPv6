#include "session.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory.h>
#include <stdlib.h>

session_t *session_open(const char *ifname)
{
    session_t *s = malloc(sizeof(session_t));
    if(s == NULL)
        return 0;

    s->sock_desc = socket(PF_PACKET, SOCK_RAW, htons(ETH_PROTOCOL_IPV6));
    if(s->sock_desc == -1)
    {
        free(s);
        return 0;
    }

    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);

    if(ioctl(s->sock_desc, SIOCGIFINDEX, &ifreq) == -1)
    {
        free(s);
        return 0;
    }
    s->ifindex = ifreq.ifr_ifindex;

    if(ioctl(s->sock_desc, SIOCGIFHWADDR, &ifreq) == -1)
    {
        free(s);
        return 0;
    }
    memcpy(s->src_addr, ifreq.ifr_hwaddr.sa_data, ETH_ADDR_LEN);

    return s;
}

int session_close(session_t *session)
{
    const int err = close(session->sock_desc);
    free(session);
    return err;
}
