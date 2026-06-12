/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linker.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 13:02:35 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 13:03:28 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LINKER_H
# define LINKER_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <signal.h>
#include <netinet/udp.h>


#include "args.h"
#include "libft.h"

int	    traceroute(t_args *args, struct sockaddr_in *dst, const char *dest_ip);
int     resolve_target(const char *target, struct sockaddr_in *dst, char *dest_ip, size_t ip_len, char *hostname, size_t hn_len);
int     recv_from_socket(void);
int     create_send_socket(void);
void	set_ttl(int send_fd, int ttl);
#endif