#include "linker.h"

int parse_positive_int(const char *str)
{
	char	*end;
	long	val;

	if (!str || *str == '\0')
		return (-1);
	val = ft_strtol(str, &end, 10);
	if (*end != '\0' || val <= 0 || val > 65535)
		return (-1);
	return((int)val);
}

t_args parse_args(int argc, char **argv)
{
	t_args	args;
	int		i;
	int		val = 0;

	/*setting args struct with default values*/
	ft_memset(&args, 0, sizeof(args));
	args.max_ttl = DEFAULT_MAX_TTL;
	args.nqueries = DEFAULT_N_QUERIES;
	args.port = DEFAULT_PORT;
	args.wait = DEFAULT_WAIT;
	args.first_ttl = DEFAULT_FIRST_TTL;

	i = 1;
	while (i < argc)
	{
		if (ft_strcmp(argv[i], "--help") == 0)
		{
			print_help(argv[0]);
			exit(0);
		}
		else if (ft_strcmp(argv[i], "-n") == 0)
			args.no_dns = 1;
		else if (ft_strcmp(argv[i], "-m") == 0)
		{
			if (i + 1 >= argc)
			{
				fd_printf(2, "%s: option requires an argument -- 'm'\n", argv[0]);
				print_usage(argv[0]);
				exit(1);
			}
			val = parse_positive_int(argv[++i]);
			if (val <= 0)
			{
				fd_printf(2, "%s: invalid argument '%s' for option '-m'\n", argv[0], argv[i]);
				exit(1);
			}
			args.max_ttl = val;
		}
		else if (ft_strcmp(argv[i], "-q") == 0)
		{
			if (i + 1 >= argc)
			{
				fd_printf(2, "%s: option requires an argument -- 'q'\n", argv[0]);
				print_usage(argv[0]);
				exit(1);
			}
			val = parse_positive_int(argv[++i]);
			if (val <= 0)
			{
				fd_printf(2, "%s: invalid argument '%s' for option '-q'\n", argv[0], argv[i]);
				exit(1);
			}
			args.nqueries = val;
		}
		else if (ft_strcmp(argv[i], "-p") == 0)
		{
			if (i + 1 >= argc)
			{
				fd_printf(2, "%s: option requires an argument -- 'p'\n", argv[0]);
				print_usage(argv[0]);
				exit(1);
			}
			val = parse_positive_int(argv[++i]);
			if (val <= 0)
			{
				fd_printf(2, "%s: invalid argument '%s' for option '-p'\n", argv[0], argv[i]);
				exit(1);
			}
			args.port = val;
		}
		else if (ft_strcmp(argv[i], "-w") == 0)
		{
			if (i + 1 >= argc)
			{
				fd_printf(2, "%s: option requires an argument -- 'w'\n", argv[0]);
				print_usage(argv[0]);
				exit(1);
			}
			val = parse_positive_int(argv[++i]);
			if (val <= 0)
			{
				fd_printf(2, "%s: invalid argument '%s' for option '-w'\n", argv[0], argv[i]);
				exit(1);
			}
			args.wait = val;
		}
		else if (ft_strcmp(argv[i], "-f") == 0)
		{
			if (i + 1 >= argc)
			{
				fd_printf(2, "%s: option requires an argument -- 'f'\n", argv[0]);
				print_usage(argv[0]);
				exit(1);
			}
			val = parse_positive_int(argv[++i]);
			if (val <= 0)
			{
				fd_printf(2, "%s: invalid argument '%s' for option '-f'\n", argv[0], argv[i]);
				exit(1);
			}
			args.first_ttl = val;
		}
		/*non-handled flag */
		else if (argv[i][0] == '-')
		{
			fd_printf(2, "%s: invalid option -- '%s'\n", argv[0], argv[i] + 1);
			print_usage(argv[0]);
			exit(1);
		}
		/*positional arg*/
		else
		{
			if (args.target != NULL)
			{
				fd_printf(2, "%s: extra argument '%s'\n", argv[0], argv[i]);
				print_usage(argv[0]);
				exit(1);
			}
			args.target = argv[i];
		}
		i++;
	}

	if (args.target == NULL)
	{
		fd_printf(2, "%s: missing host argument (mandatory)\n", argv[0]);
		print_usage(argv[0]);
		exit(1);
	}

	// tll check
	if (args.first_ttl > args.max_ttl)
	{
		fd_printf(2, "%s: first ttl (%d) must be <= max_ttl (%d)", argv[0], args.first_ttl, args.max_ttl);
		exit(1);
	}
	return (args);
}

