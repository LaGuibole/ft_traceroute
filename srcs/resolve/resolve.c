#include "linker.h"

t_host	resolve_host(const char *target)
{
	t_host			host;
	struct addrinfo	hints;
	struct addrinfo *res;
	int				status;

	ft_memset(&host, 0, sizeof(t_host));
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM; // udp

	status = getaddrinfo(target, NULL, &hints, &res);
	if (status != 0)
	{
		fd_printf(2, "ft_traceroute: %s: %s\n", target, gai_strerror(status));
		exit(1);
	}

	host.addr = *(struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, &host.addr.sin_addr, host.ip_str, sizeof(host.ip_str));
	ft_strlcpy(host.fqdn, target, sizeof(host.fqdn));

	freeaddrinfo(res);
	return (host);
}