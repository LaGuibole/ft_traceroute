#ifndef ARGS_H
# define ARGS_H

#define	DEFAULT_MAX_TTL		30
#define	DEFAULT_N_QUERIES	3
#define	DEFAULT_PORT		33434
#define DEFAULT_WAIT		5
#define	DEFAULT_FIRST_TTL	1

typedef struct s_args
{
	// mandatory
	char	*target;
	int		help;
	// bonus ? a voir, on le parse au cas ou 
	int		max_ttl; // -m <ttl>
	int		nqueries; // -q <n>
	int		port; // -p <port>
	int		wait; // -w <sec>
	int		first_ttl; // -f <ttl>
	int		no_dns; // -n
	// char	*iface
	// char	*src_ip;
}	t_args;


t_args	parse_args(int argc, char **argv);
void	print_help(char *prog_name);
void	print_usage(char *prog_name);

#endif