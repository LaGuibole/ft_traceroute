/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_addr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 15:25:22 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 15:50:28 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "linker.h"

/// @brief Resout une cible (IPv4) et remplit la structure fournie (addrinfo hints)
/// @param target Nom d'hote ou adresse a resoudre
/// @param out Pointeur vers une structure socckaddr_in a remplir avec l'adresse resolue
/// @return 0 si succes, sinon -1
int resolve_target(const char* target, struct sockaddr_in *out)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    ret = getaddrinfo(target, NULL, &hints, &res);
    if (ret != 0)
    {
        fprintf(stderr, "ft_ping: unknown host\n");
        return -1;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    memcpy(out, addr, sizeof(struct sockaddr_in));

    freeaddrinfo(res);
    return 0;
}