#include "linker.h"

/**
 * @brief Send a UDP probe toward the destination host.
 *
 * Builds a UDP probe using a destination port derived from the base
 * port and the provided offset. The probe payload is filled with
 * zeroes and transmitted through the given socket.
 *
 * The transmission timestamp is recorded immediately before sending
 * and is later used to compute the round-trip time (RTT).
 *
 * @param send_fd UDP socket used to send probes.
 * @param dst Destination IPv4 socket address.
 * @param base_port Base destination port configured by the user.
 * @param port_offset Offset added to the base port to uniquely identify
 *                    each probe.
 * @param tv_sent Pointer receiving the send timestamp.
 *
 * @return The destination port used for the probe on success.
 * @return -1 if the probe could not be sent.
 */
static int send_probe(int send_fd, struct sockaddr_in *dst, int base_port, int port_offset, struct timeval *tv_sent)
{
	char				payload[40];
	struct sockaddr_in 	target;
	int					dest_port;

	ft_memset(payload, 0, sizeof(payload));
	dest_port = base_port + port_offset;

	target = *dst;
	target.sin_port = htons((uint16_t)dest_port);
	target.sin_family = AF_INET;

	gettimeofday(tv_sent, NULL);
	if (sendto(send_fd, payload, sizeof(payload), 0, (struct sockaddr *)&target, sizeof(target)) < 0)
	{
		fd_printf(2, "ft_traceroute: sendto: %s\n", strerror(errno));
		return (-1);
	}
	return (dest_port);
}

/**
 * @brief Validate and analyze an ICMP response packet.
 *
 * Parses a received ICMP packet and verifies that it corresponds
 * to one of our previously sent UDP probes by inspecting the
 * encapsulated IP and UDP headers.
 *
 * Supported ICMP responses:
 * - ICMP Time Exceeded
 * - ICMP Destination Unreachable (Port Unreachable)
 *
 * Packets not matching the expected destination port are ignored.
 *
 * @param buf Buffer containing the received packet.
 * @param len Size of the received packet.
 * @param reply_ip Pointer receiving the source IP address of the
 *                 responding router or destination host.
 * @param expected_port Destination port associated with the probe.
 *
 * @return 1 if an ICMP Time Exceeded message was received.
 * @return 2 if an ICMP Port Unreachable message was received.
 * @return 0 if the packet is invalid or does not belong to us.
 */
static int	parse_icmp_reply(char *buf, ssize_t len, struct in_addr *reply_ip, int expected_port)
{
	struct iphdr	*outer_ip;
	struct icmphdr	*icmp;
	struct iphdr	*inner_ip;
	struct udphdr	*inner_udp;
	int				outer_ip_len;
	int				inner_ip_len;
 
	if (len < (ssize_t)(sizeof(struct iphdr) + sizeof(struct icmphdr)))
		return (0);
 
	outer_ip = (struct iphdr *)buf;
	outer_ip_len = outer_ip->ihl * 4;
 
	if (outer_ip->protocol != IPPROTO_ICMP)
		return (0);
 
	icmp = (struct icmphdr *)(buf + outer_ip_len);
 
	if (icmp->type != ICMP_TIME_EXCEEDED && icmp->type != ICMP_DEST_UNREACH)
		return (0);
 
	inner_ip = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
	inner_ip_len = inner_ip->ihl * 4;
 
	if (inner_ip->protocol != IPPROTO_UDP)
		return (0);
 
	inner_udp = (struct udphdr *)((char *)inner_ip + inner_ip_len);
 
	if (ntohs(inner_udp->dest) != (uint16_t)expected_port)
		return (0);
 
	reply_ip->s_addr = outer_ip->saddr;
 
	if (icmp->type == ICMP_TIME_EXCEEDED)
		return (1);
	if (icmp->type == ICMP_DEST_UNREACH && icmp->code == ICMP_PORT_UNREACH)
		return (2);
	return (0);
}

/**
 * @brief Wait for an ICMP response to a previously sent probe.
 *
 * Uses select() to wait for incoming ICMP packets during the
 * configured timeout period. When a packet is received, it is
 * validated and matched against the expected probe.
 *
 * If a matching response is found, the round-trip time is computed
 * from the recorded send timestamp.
 *
 * @param recv_fd Raw ICMP receiving socket.
 * @param tv_sent Probe transmission timestamp.
 * @param wait_sec Timeout in seconds.
 * @param reply_ip Pointer receiving the responder IP address.
 * @param rtt_ms Pointer receiving the calculated RTT in milliseconds.
 * @param expected_port Destination port associated with the probe.
 *
 * @return 2 if the destination host was reached.
 * @return 1 if an intermediate router replied.
 * @return 0 on timeout or unrelated packet.
 * @return -1 on system error.
 */
static int	wait_for_reply(int recv_fd, struct timeval *tv_sent,
							int wait_sec, struct in_addr *reply_ip,
							double *rtt_ms, int expected_port)
{
	char			buf[512];
	struct timeval	tv_now;
	struct timeval	timeout;
	fd_set			readfds;
	ssize_t			n;
	int				ret;
 
	timeout.tv_sec = wait_sec;
	timeout.tv_usec = 0;
 
	FD_ZERO(&readfds);
	FD_SET(recv_fd, &readfds);
 
	ret = select(recv_fd + 1, &readfds, NULL, NULL, &timeout);
	if (ret < 0)
	{
		fd_printf(2, "ft_traceroute: select: %s\n", strerror(errno));
		return (-1);
	}
	if (ret == 0)
		return (0); /* timeout */
 
	n = recvfrom(recv_fd, buf, sizeof(buf), 0, NULL, NULL);
	if (n < 0)
		return (-1);
 
	gettimeofday(&tv_now, NULL);
 
	ret = parse_icmp_reply(buf, n, reply_ip, expected_port);
	if (ret <= 0)
		return (0); 
 
	*rtt_ms = (double)(tv_now.tv_sec - tv_sent->tv_sec) * 1000.0
			+ (double)(tv_now.tv_usec - tv_sent->tv_usec) / 1000.0;
	return (ret);
}

/**
 * @brief Resolve an IPv4 address into a hostname.
 *
 * Performs a reverse DNS lookup on the supplied IPv4 address unless
 * DNS resolution has been disabled.
 *
 * If reverse resolution fails or is disabled, the function falls back
 * to the dotted-decimal IPv4 representation.
 *
 * @param ip IPv4 address to resolve.
 * @param buf Buffer receiving the hostname or IP string.
 * @param buf_len Size of the output buffer.
 * @param no_dns Non-zero to disable reverse DNS lookups.
 */
static void	resolve_hostname(struct in_addr *ip, char *buf, size_t buf_len,
								int no_dns)
{
	struct sockaddr_in	sa;
	char				host[256];
 
	ft_memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr = *ip;
 
	if (!no_dns)
	{
		if (getnameinfo((struct sockaddr *)&sa, sizeof(sa),
				host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
		{
			ft_strlcpy(buf, host, buf_len);
			return ;
		}
	}
	ft_strlcpy(buf, inet_ntoa(*ip), buf_len);
}

/**
 * @brief Display traceroute results for a single hop.
 *
 * Prints the hop number, hostname/IP address and RTT values for all
 * probes sent with the same TTL.
 *
 * Timed-out probes are displayed as '*'. Hostname and IP information
 * are only printed when a response is available.
 *
 * @param ttl Current hop TTL value.
 * @param replies Array containing responder IP addresses.
 * @param rtts Array containing probe RTT values in milliseconds.
 * @param nqueries Number of probes sent for this hop.
 * @param no_dns Non-zero to disable reverse DNS lookups.
 */
static void	print_hop(int ttl, struct in_addr replies[], double rtts[],
						int nqueries, int no_dns)
{
	char	hostname[256];
	char	ip_str[INET_ADDRSTRLEN];
	int		q;
	int		printed_host;
 
	printf("%2d ", ttl);
	printed_host = 0;
 
	q = 0;
	while (q < nqueries)
	{
		if (rtts[q] < 0)
		{
			printf(" *");
		}
		else
		{
			inet_ntop(AF_INET, &replies[q], ip_str, sizeof(ip_str));
 
			if (!printed_host
				|| (q > 0 && replies[q].s_addr != replies[q - 1].s_addr))
			{
				resolve_hostname(&replies[q], hostname, sizeof(hostname), no_dns);
				if (!no_dns)
					printf(" %s (%s)", hostname, ip_str);
				else
					printf(" %s", ip_str);
				printed_host = 1;
			}
			printf("  %.3f ms", rtts[q]);
		}
		q++;
	}
	printf("\n");
}

/**
 * @brief Execute a UDP-based traceroute toward the target host.
 *
 * Creates the required sending and receiving sockets, then sends
 * UDP probes with increasing TTL values in order to discover each
 * router along the path to the destination.
 *
 * For every hop:
 * - TTL is configured on the sending socket.
 * - Multiple probes are transmitted.
 * - ICMP responses are collected.
 * - Round-trip times are measured.
 * - Results are displayed.
 *
 * The trace stops when the destination host returns an ICMP
 * Port Unreachable message or when the maximum TTL is reached.
 *
 * @param args Parsed program configuration.
 * @param dst Destination IPv4 socket address.
 * @param dest_ip Destination IP address as a string.
 *
 * @return 0 if the destination was reached.
 * @return 1 if the maximum TTL was reached before reaching the target.
 */
int	traceroute(t_args *args, struct sockaddr_in *dst,
				const char *dest_ip)
{
	int				send_fd;
	int				recv_fd;
	int				ttl;
	int				q;
	int				port_offset;
	int				dest_port;
	int				reply_type;
	int				reached;
	double			rtts[64]; 
	struct in_addr	replies[64];
	struct timeval	tv_sent;
 
	send_fd = create_send_socket();
	recv_fd = recv_from_socket();
	port_offset = 0;
	reached = 0;
 
	printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n",
		args->target, dest_ip, args->max_ttl, (int)(sizeof(struct iphdr) + 40));
 
	ttl = args->first_ttl;
	while (ttl <= args->max_ttl && !reached)
	{
		q = 0;
		while (q < args->nqueries)
		{
			rtts[q] = -1.0;
			replies[q].s_addr = 0;
			q++;
		}
 
		set_ttl(send_fd, ttl);
 
		q = 0;
		while (q < args->nqueries)
		{
			dest_port = send_probe(send_fd, dst,
						args->port, port_offset, &tv_sent);
			port_offset++;
			if (dest_port < 0)
			{
				q++;
				continue ;
			}
 
			reply_type = wait_for_reply(recv_fd, &tv_sent,
							args->wait, &replies[q],
							&rtts[q], dest_port);
 
			if (reply_type == 2)
				reached = 1;
			q++;
		}
 
		print_hop(ttl, replies, rtts, args->nqueries, args->no_dns);
		ttl++;
	}
 
	close(send_fd);
	close(recv_fd);
	return (reached ? 0 : 1);
}