#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

#include "linker.h"

#define		PACKET_SIZE		60
#define		IP_HDR_SIZE		20
#define		ICMP_HDR_SIZE	8

typedef struct s_host
{
	char				fqdn[NI_MAXHOST];
	char				ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in	addr;
} t_host;

t_host	resolve_host(const char *target);
void	print_header(const t_host *host, const t_args *args);

#endif
