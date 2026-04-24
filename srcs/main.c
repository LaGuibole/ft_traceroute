/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 13:02:04 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 13:02:14 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "linker.h"

int main(int argc, char **argv)
{
	t_args	args;

	args = parse_args(argc, argv);	

	ft_printf("target			: %s\n", args.target);
	ft_printf("max_ttl			: %d\n", args.max_ttl);
	ft_printf("nqueries		: %d\n", args.nqueries);
	ft_printf("port			: %d\n", args.port);
	ft_printf("wait			: %d\n", args.wait);
	ft_printf("first_ttl		: %d\n", args.first_ttl);
	ft_printf("no_dns			: %d\n", args.no_dns);

	return (0);
}