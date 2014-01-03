#include "session.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory.h>
#include <stdlib.h>

#include <errno.h>

session_t *session_open(const char *interface, const uint8_t src_ip[], uint16_t port, protocol_t protocol);
{
    // We request kernel to pass us only frames with protocol set to IPv6.
    // We could filter out non-IP packets manually as well, so it doesn't really
    // matter.
    const uint16_t ETH_PROTOCOL = htons(ETH_PROTOCOL_IPV6);

    session_t *s = malloc(sizeof(session_t));
    if(s == 0)
        return 0;

    // Create the socket.
    s->sock_desc = socket(AF_PACKET, SOCK_RAW, ETH_PROTOCOL);
    if(s->sock_desc == -1)
    {
        free(s);
        return 0;
    }

    // Create ifreq object to fetch details about our network interface
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);

    // Get interface's index
    if(ioctl(s->sock_desc, SIOCGIFINDEX, &ifreq) == -1)
    {
        session_close(s);
        return 0;
    }
    const int ifindex = ifreq.ifr_ifindex;

    // Get interface's HW addr
    if(ioctl(s->sock_desc, SIOCGIFHWADDR, &ifreq) == -1)
    {
        session_close(s);
        return 0;
    }
    memcpy(s->src_addr, ifreq.ifr_hwaddr.sa_data, ETH_ADDR_LEN);

    // Set interface's IP addr
    memcpy(s->src_ip, src_ip, IP_ADDR_LEN);

    // Prepare the sockaddr_ll struct for binding
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = ETH_PROTOCOL;
    addr.sll_ifindex = ifindex;

    // Bind the socket to the interface
    if(bind(s->sock_desc, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    {
        session_close(s);
        return 0;
    }

    return s;
}

int session_close(session_t *session)
{
    const int err = close(session->sock_desc);
    free(session);
    return err;
}
