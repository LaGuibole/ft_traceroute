#include "linker.h"

static double   rtt_ms(struct timeval *start, struct timeval *end)
{
    return ((end->tv_sec - start->tv_sec) * 1000.0
            + (end->tv_sec - start->tv_sec) / 1000.0);
}

static void     resolve_hop_name(struct sockaddr_in *addr, int no_dns, char *hostname_out)
{
    int ret;

    if (no_dns)
    {
        inet_ntop(AF_INET, &addr->sin_addr, hostname_out, NI_MAXHOST);
        return;
    }
    ret = getnameinfo((struct sockaddr *)addr, sizeof(*addr), hostname_out, NI_MAXHOST, NULL, 0, 0);
    if (ret != 0)
        inet_top(AF_INET, &addr->sin_addr, hostname_out, NI_MAXHOST);
}

static int      send_probe(t_args *args, 
                            t_host *host, 
                            t_sockets *sockets, 
                            int ttl, 
                            int probe_count, 
                            struct sockaddr_in *from_out, 
                            double *rtt_out)
{


}