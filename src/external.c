#include "external.h"

#include "common.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory.h>
#include <string.h>

int hw_session_open(const char interface[])
{
	// We request kernel to pass us only frames with protocol set to IPv6.
    // We could filter out non-IP packets manually as well, so it doesn't really
    // matter.
    const uint16_t ETH_PROTOCOL = htons(ETH_PROTOCOL_IPV6);

    const int sock_desc = socket(AF_PACKET, SOCK_RAW, ETH_PROTOCOL);
    if(sock_desc == -1)
        return -1;

    // Create ifreq object to fetch details about our network interface
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, interface, IFNAMSIZ);

    // Get interface's index
    if(ioctl(sock_desc, SIOCGIFINDEX, &ifreq) == -1)
    {
        hw_session_close(sock_desc);
        return -1;
    }

    // Prepare the sockaddr_ll struct for binding
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = ETH_PROTOCOL;
    addr.sll_ifindex = ifreq.ifr_ifindex;

    // Bind the socket to the interface
    if(bind(sock_desc, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    {
    	hw_session_close(sock_desc);
    	return -1;
    }

    return sock_desc;
}

int hw_interface_addr(int session_id, const char interface[], uint8_t addr[])
{
    // Create ifreq object to fetch details about our network interface
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, interface, IFNAMSIZ);

    // Get interface's HW addr
    if(ioctl(session_id, SIOCGIFHWADDR, &ifreq) == -1)
        return -1;

    memcpy(addr, ifreq.ifr_hwaddr.sa_data, ETH_ADDR_LEN);
    return 0;
}

int hw_session_close(int session_id)
{
	return close(session_id);
}

size_t hw_send(int session_id, uint8_t data[], size_t data_len)
{
	return send(session_id, data, data_len, 0);
}

size_t hw_recv(int session_id, uint8_t buffer[], size_t buffer_len)
{
	return recv(session_id, buffer, buffer_len, 0);
}

int16_t network_s(int16_t value)
{
	return htons(value);
}

int32_t network_l(int32_t value)
{
	return htonl(value);
}
