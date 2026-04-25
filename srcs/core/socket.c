#include "linker.h"

t_sockets   init_sockets()
{
    t_sockets   *sockets;

    sockets.send_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockets.send_fd < 0)
    {
        fd_printf(2, "ft_traceroute: socket (send): %s\n", strerror(errno));
        exit(1);
    }
    sockets.recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockets.recv_fd < 0)
    {
        close(sockets.recv_fd);
        fd_printf(2, "ft_traceroute: socket (send): %s\n", strerror(errno));
        exit(1);
    }
    return (sockets);
}

void    close_sockets(t_sockets *sockets)
{
    if (sockets->send_fd >= 0)
        close(sockets->send_fd);
    if (sockets->recv_fd >= 0)
        close(sockets->recv_fd);
    sockets->send_fd = -1;
    sockets->recv_fd = -1;
}