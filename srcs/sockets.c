/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sockets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 15:44:41 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 15:59:00 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "linker.h"

/// @brief Cette fonction cree les sockets pour les deux protocoles UDP / ICMP
/// @param sockets Un pointeur vers le ctx sockets
/// @return -1 sur une erreur de creation, 0 si succes
int create_sockets(t_sockets *sockets)
{
    if (!sockets)
        return -1;
    sockets->icmpfd = -1;
    sockets->udpfd = -1;

    int udpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpfd < 0)
    {
        perror("UDP socket creation failed badly...\n");
        return -1;
    }

    int icmpfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (icmpfd < 0)
    {
        perror("ICMP socket creation failed badly...\n");
        close(udpfd);
        return -1;
    }

    sockets->udpfd = udpfd;
    sockets->icmpfd = icmpfd;
    return 0;
}

/// @brief Cette fonction verifie que le socket etait ouvert,le ferme et set a -1
/// @param sockets Un pointeur vers le ctx sockets.
void close_sockets(t_sockets *sockets)
{
    if (!sockets)
        return -1;
    if (sockets->udpfd >= 0)
    {
        close(sockets->udpfd);
        sockets->udpfd = -1;
    }
    if (sockets->icmpfd >= 0)
    {
        close(sockets->icmpfd);
        sockets->icmpfd = -1;
    }
}