#include "linker.h"

int recv_from_socket(void)
{
	int	fd;

	fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (fd < 0)
	{
		fd_printf(2, "ft_traceroute: socket (recv): %s\n", strerror(errno));
		fd_printf(2, 				"(are you running as root ?)\n");
		exit(1);
	}
	return (fd);
}

int create_send_socket(void)
{
	int fd;
	 
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0)
	{
		fd_printf(2, "ft_traceroute: socket (send): %s\n", strerror(errno));
		exit(1);
	}
	return (fd);
}

void	set_ttl(int send_fd, int ttl)
{
	if (setsockopt(send_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		fd_printf(2, "ft_traceroute: setsockopt IP_TTL: %s\n", strerror(errno));
		exit(1);
	}
}