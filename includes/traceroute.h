/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 15:20:25 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 16:24:55 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRACEROUTE_H
# define TRACEROUTE_H

#define OPTION_COUNT 2

typedef struct s_sockets {
    int udpfd;
    int icmpfd;
} t_sockets;

typedef struct s_request_udp {
    int udpfd;
    struct sockaddr_in dest;
    uint16_t dest_port;
    const void* payload;
    size_t payload_len;
} t_request_udp;

typedef struct s_reply_icmp {
    struct sockaddr_in src_addr;
    int icmp_type;
    int icmp_code;
    uint16_t mathed_dest_port;
    struct timeval recv_time;
} t_reply_icmp;

#endif