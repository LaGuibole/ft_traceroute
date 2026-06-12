#include "linker.h"

/**
 * @brief Resolve a hostname or IPv4 address into a sockaddr_in structure.
 *
 * Uses getaddrinfo() to resolve the provided target and fills the
 * destination socket address structure with the resulting IPv4 address.
 * The resolved IP address is also converted to its textual representation
 * and stored in the provided buffer.
 *
 * The original target string is copied into the hostname buffer for
 * later display purposes.
 *
 * Resolution is restricted to IPv4 (AF_INET) and UDP sockets
 * (SOCK_DGRAM).
 *
 * On failure, an error message is printed to stderr and the program
 * terminates.
 *
 * @param target Hostname or IPv4 address to resolve.
 * @param dst Pointer to the destination sockaddr_in structure to fill.
 * @param dest_ip Buffer receiving the resolved IPv4 address as a string.
 * @param ip_len Size of the dest_ip buffer.
 * @param hostname Buffer receiving a copy of the original target string.
 * @param hn_len Size of the hostname buffer.
 *
 * @return 0 on success.
 *
 * @note This function currently never returns on failure and exits the
 *       program directly.
 */
int resolve_target(const char *target, struct sockaddr_in *dst, char *dest_ip, size_t ip_len, char *hostname, size_t hn_len)
{
	struct addrinfo hints;
	struct addrinfo *res;
	int ret;

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	ret = getaddrinfo(target, NULL, &hints, &res);
	if (ret != 0)
	{
		fd_printf(2, "ft_traceroute: %s: %s\n", target, gai_strerror(ret));
		exit(1);
	}

	ft_memset(dst, 0, sizeof(*dst));
	*dst = *(struct sockaddr_in *)res->ai_addr;

	if (!inet_ntop(AF_INET, &dst->sin_addr, dest_ip, (socklen_t)ip_len))
	{
		fd_printf(2, "ft_traceroute: inet_ntop failed\n");
		freeaddrinfo(res);
		exit(1);
	}

	ft_strlcpy(hostname, target, hn_len);
	freeaddrinfo(res);
	return (0);
}