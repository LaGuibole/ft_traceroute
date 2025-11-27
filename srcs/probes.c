/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   probes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 16:11:54 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 16:30:54 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "linker.h"

/// @brief Envoi d'un probe depuis le ctx UDP 
/// @param ctx Le contexte UDP (fourni dans traceroute.h)
/// @return 0 si l'envoi reussi, sinon -1
int send_probe(t_request_udp *ctx)
{
    if (!ctx || ctx->udpfd < 0)
        return -1;
    
    struct sockaddr_in to = ctx->dest;
    to.sin_port = htons(ctx->dest_port);

    //buffer par defaut si payload non fourni
    char default_buffer[32];
    const void * buffer = ctx->payload;
    size_t len = ctx->payload_len;
    
    if (!buffer || len == 0)
    {
        memset(default_buffer, 0, sizeof(default_buffer));
        buffer = default_buffer;
        len = sizeof(default_buffer);
    }

    ssize_t ret = sendto(ctx->udpfd, buffer, len, 0, (struct sockaddr *)&to, sizeof(to));
    if (!ret)
        return -1;
    return 0;
}