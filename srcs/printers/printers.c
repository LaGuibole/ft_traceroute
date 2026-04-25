#include "linker.h"

void print_usage(char *prog_name)
{
	fd_printf(2, "Usage: %s [options] <host>\n", prog_name);
	fd_printf(2, "Try '%s --help' for more information.\n", prog_name);
}

void print_help(char *prog_name)
{
	ft_printf("Usage: %s [options] <host>\n\n", prog_name);
	ft_printf("Options :\n");
	ft_printf("  --help 			Print this help message\n");
	ft_printf("  -f <first ttl> 		Start from the <first ttl> hop (default: %d)\n", DEFAULT_FIRST_TTL);
	ft_printf("  -m <max_ttl> 			Max number of hops (default: %d)\n", DEFAULT_MAX_TTL);
	ft_printf("  -n				Do not resolve IP addresses to hostnames\n");
	ft_printf("  -p <port>			Base UDP port (default: %d)\n", DEFAULT_PORT);
	ft_printf("  -q <nqueries>			Number of probes per hop (default: %d)\n", DEFAULT_N_QUERIES);
	ft_printf("  -w <wait>			Probe timeout in seconds (default: %d)\n", DEFAULT_WAIT);
}

void print_header(const t_host *host, const t_args *args)
{
	ft_printf("traceroute to %s (%s), %d hops max, %d byte packets\n", host->fqdn, host->ip_str, args->max_ttl, PACKET_SIZE);
}