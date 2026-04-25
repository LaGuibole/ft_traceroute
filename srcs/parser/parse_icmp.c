#include "linker.h"

int parse_icmp(char *buffer, int len, struct sockaddr_in *from, int base_port, int probe_count)
{
    struct iphdr    *ip;
    struct icmphdr  *icmp;
    struct iphdr    *inner_ip;
    struct udphdr   *udp;
    int             ip_hdr_len;
    int             expected_port;

    (void)from;

    if (len < IP_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE + UDP_HDR_SIZE)
        return (-1);
    
    ip          = (struct iphdr *)buffer;
    ip_hdr_len  = ip->ihl * 4;
    icmp        = (struct icmphdr *)(buffer + ip_hdr_len);
    
    if (icmp->type != ICMP_TIME_EXCEEDED && icmp->type != ICMP_DEST_UNREACH)
        return (-1);
    if (icmp->type == ICMP_DEST_UNREACH && icmp->code != ICMP_PORT_UNREACH)
        return (-1);
    
    inner_ip    = (struct iphdr *)(buffer + ip_hdr_len + ICMP_HDR_SIZE);
    if (inner_ip->protocol != IPPROTO_UDP)
        return (-1);

    udp = (struct udphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
    expected_port = base_port + probe_count;
    if (ntohs(udp->dest) != expected_port)
        return (-1);
    
    return (icmp->type);
}