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

typedef struct s_sockets 
{
	int		send_fd;	// SOCK_DGRAM + IPPROTO_UDP 
	int 	recv_fd;	// SOCK_RAW + IPPROTO_ICMP
} t_sockets;

t_sockets   init_sockets(void);
void    	close_sockets(t_sockets *sockets);

int parse_icmp(char *buffer, int len, struct sockaddr_in *from, int base_port, int probe_count);


#endif
