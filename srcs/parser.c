/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 13:06:31 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 16:31:15 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/* ---------------------------- Static prototypes --------------------------- */

static Option *find_option(
	ArgParser *parser,
	const char *flag);

static ArgParseResult parse_int_safe(
	const char *s,
	int *out);

static ArgParseResult parse_double_safe(
	const char *s,
	double *out);

static ArgParseResult assign_option_value(
	Option *opt,
	char *value_from_eq,
	int argc,
	char *argv[],
	int *i);

static ArgParseResult bind_positionals(
	ArgParser *parser);

/* ------------------------- Header implementations ------------------------- */

void init_arg_parser(
	ArgParser *parser,
	const char *program_name)
{
	parser->option_count = 0;
	parser->positional_count = 0;
	parser->positional_def_count = 0;
	parser->options[0] = (Option){0};
	parser->positional_defs[0] = (Positional){0};
	parser->positional_args[0] = NULL;
	parser->program_name = strdup(program_name);
	parser->last_error = ARGPARSE_OK;
	parser->last_error_arg = NULL;
}

ArgParseResult add_option(
	ArgParser *parser,
	const Option *opt_def)
{
	if (parser->option_count >= MAX_OPTIONS)
		return (parser->last_error = ARGPARSE_ERR_TOO_MANY_OPTIONS);

	Option *dst = &parser->options[parser->option_count++];

	dst->short_flag = opt_def->short_flag ? strdup(opt_def->short_flag) : NULL;
	dst->long_flag = opt_def->long_flag ? strdup(opt_def->long_flag) : NULL;
	dst->description = opt_def->description ? strdup(opt_def->description) : NULL;

	if ((opt_def->short_flag && !dst->short_flag) ||
		(opt_def->long_flag && !dst->long_flag) ||
		(opt_def->description && !dst->description))
		return (parser->last_error = ARGPARSE_ERR_ALLOC);

	dst->type = opt_def->type;
	dst->value = opt_def->value;

	return parser->last_error = ARGPARSE_OK;
}

ArgParseResult parse_arguments(
	ArgParser *parser,
	int argc,
	char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		char *arg = argv[i];
		char *eq = strchr(arg, '=');
		char *value_from_eq = NULL;

		if (eq != NULL)
		{
			*eq = '\0';
			value_from_eq = eq + 1;
		}

		Option *opt = find_option(parser, arg);

		if (eq != NULL)
			*eq = '=';

		if (opt != NULL)
		{
			ArgParseResult r = assign_option_value(opt, value_from_eq,
												   argc, argv, &i);
			if (r != ARGPARSE_OK)
				return (parser->last_error = r,
					 parser->last_error_arg = value_from_eq ? value_from_eq : argv[i],
					 r);
		}
		else
		{
			if (argv[i][0] == '-' && (argv[i][1] != '\0' || parser->positional_count == parser->positional_def_count))
				return (parser->last_error = ARGPARSE_ERR_UNKNOWN_OPTION,
					 parser->last_error_arg = argv[i],
					 ARGPARSE_ERR_UNKNOWN_OPTION);
			if (parser->positional_count >= MAX_POSITIONAL)
				return (parser->last_error = ARGPARSE_ERR_TOO_MANY_POSITIONALS);
			parser->positional_args[parser->positional_count++] = argv[i];
		}
	}
	return bind_positionals(parser);
}

void print_usage(const ArgParser *parser)
{
	int max_flag_len = 0;
	int max_pos_name_len = 0;
	char positional_buf[32 * MAX_POSITIONAL] = {0};

	for (int i = 0; i < parser->positional_def_count; ++i)
	{
		snprintf(positional_buf + strlen(positional_buf),
				 sizeof(positional_buf) - strlen(positional_buf),
				 " <%s>",
				 parser->positional_defs[i].name ? parser->positional_defs[i].name : "arg");
	}
	printf("Usage: %s%s [OPTIONS ...]\n", parser->program_name, positional_buf);

	for (int i = 0; i < parser->option_count; ++i)
	{
		const Option *opt = &parser->options[i];
		int len = 0;

		if (opt->short_flag)
			len += (int)strlen(opt->short_flag);
		if (opt->short_flag && opt->long_flag)
			len += 2;
		if (opt->long_flag)
			len += (int)strlen(opt->long_flag);
		if (len > max_flag_len)
			max_flag_len = len;
	}

	for (int i = 0; i < parser->positional_def_count; ++i)
	{
		const Positional *def = &parser->positional_defs[i];
		int len = def->name ? (int)strlen(def->name) : 0;
		if (len > max_pos_name_len)
			max_pos_name_len = len;
	}

	if (parser->option_count > 0)
	{
		printf("Options:\n");
		for (int i = 0; i < parser->option_count; ++i)
		{
			const Option *opt = &parser->options[i];
			char flags_buf[128] = {0};

			if (opt->short_flag && opt->long_flag)
				snprintf(flags_buf, sizeof(flags_buf), "%s, %s",
						 opt->short_flag, opt->long_flag);
			else if (opt->short_flag)
				snprintf(flags_buf, sizeof(flags_buf), "%s", opt->short_flag);
			else if (opt->long_flag)
				snprintf(flags_buf, sizeof(flags_buf), "    %s%s", opt->long_flag, 
					opt->type == ARGTYPE_INT ? "=N" : 
					opt->type == ARGTYPE_DOUBLE ? "=F" : "");

			printf("  %-*s", max_flag_len, flags_buf);
			if (opt->description)
				printf("  %s", opt->description);
			printf("\n");
		}
	}

	if (parser->positional_def_count > 0)
	{
		printf("\nPositional arguments:\n");
		for (int i = 0; i < parser->positional_def_count; ++i)
		{
			const Positional *def = &parser->positional_defs[i];
			printf("  %-*s", max_pos_name_len,
				   def->name ? def->name : "");
			if (def->description)
				printf("  %s", def->description);
			if (def->required)
				printf(" (required)");
			printf("\n");
		}
	}
}

ArgParseResult print_argparse_error(ArgParser *parser)
{
	fprintf(stderr, "%s: Error: ", parser->program_name);
	if (parser->last_error == ARGPARSE_ERR_TOO_MANY_OPTIONS)
		fprintf(stderr, "too many options defined\n\n");
	else if (parser->last_error == ARGPARSE_ERR_TOO_MANY_POSITIONALS)
		fprintf(stderr, "too many positional arguments defined\n\n");
	else if (parser->last_error == ARGPARSE_ERR_MISSING_VALUE)
		fprintf(stderr, "missing value for option %s\n\n", parser->last_error_arg ? parser->last_error_arg : "");
	else if (parser->last_error == ARGPARSE_ERR_INVALID_INT)
		fprintf(stderr, "invalid integer value %s\n\n", parser->last_error_arg);
	else if (parser->last_error == ARGPARSE_ERR_INVALID_DOUBLE)
		fprintf(stderr, "invalid double value %s\n\n", parser->last_error_arg);
	else if (parser->last_error == ARGPARSE_ERR_ALLOC)
		fprintf(stderr, "memory allocation failure\n\n");
	else if (parser->last_error == ARGPARSE_ERR_UNKNOWN_OPTION)
		fprintf(stderr, "unrecognized option -- '%s'\n\n", parser->last_error_arg);
	else
		fprintf(stderr, "unknown error\n\n");
	return parser->last_error;
}

ArgParseResult add_options(
	ArgParser *parser,
	const Option *opts,
	int count
)
{
	for (int i = 0; i < count; ++i)
	{
		ArgParseResult r = add_option(parser, &opts[i]);
		if (r != ARGPARSE_OK)
			return (parser->last_error = r);
	}
	return parser->last_error = ARGPARSE_OK;
}

void free_arg_parser(ArgParser *parser)
{
	for (int i = 0; i < parser->option_count; ++i)
	{
		Option *opt = &parser->options[i];

		free(opt->short_flag);
		free(opt->long_flag);
		free(opt->description);
		if (opt->type == ARGTYPE_STRING)
			free(opt->value);
		opt->short_flag = NULL;
		opt->long_flag = NULL;
		opt->description = NULL;
	}
	parser->option_count = 0;
	parser->positional_count = 0;
	if (parser->program_name)
	{
		free(parser->program_name);
		parser->program_name = NULL;
	}
}

Option build_option(
	const char *short_flag,
	const char *long_flag,
	ArgType type,
	void *value,
	const char *description)
{
	Option res = {0};


	res.short_flag = (char *)short_flag;
	res.long_flag = (char *)long_flag;
	res.type = type;
	res.value = value;
	res.description = (char *)description;

	return res;
}

Option build_flag_option(
	const char *short_flag,
	const char *long_flag,
	byte *value,
	const char *description)
{
	return build_option(short_flag, long_flag, ARGTYPE_FLAG, value, description);
}

Option build_int_option(
	const char *short_flag,
	const char *long_flag,
	int *value,
	const char *description)
{
	return build_option(short_flag, long_flag, ARGTYPE_INT, value, description);
}

Option build_str_option(
	const char *short_flag,
	const char *long_flag,
	char **value,
	const char *description)
{
	return build_option(short_flag, long_flag, ARGTYPE_STRING, value, description);
}

Option build_double_option(
	const char *short_flag,
	const char *long_flag,
	double *value,
	const char *description)
{
	return build_option(short_flag, long_flag, ARGTYPE_DOUBLE, value, description);
}

ArgParseResult add_positional(
	ArgParser *parser,
	const char *name,
	char **value,
	const char *description,
	int required)
{
	if (parser->positional_def_count >= MAX_POSITIONAL)
		return (parser->last_error = ARGPARSE_ERR_TOO_MANY_POSITIONALS);

	Positional *def = &parser->positional_defs[parser->positional_def_count++];
	def->name = name;
	def->value = value;
	def->description = description;
	def->required = required ? 1 : 0;
	return ARGPARSE_OK;
}

/* ------------------------- Static implementations ------------------------- */

static Option *find_option(
	ArgParser *parser,
	const char *flag)
{
	for (int i = 0; i < parser->option_count; ++i)
	{
		Option *opt = &parser->options[i];

		if ((opt->short_flag && strcmp(flag, opt->short_flag) == 0) ||
			(opt->long_flag && strcmp(flag, opt->long_flag) == 0))
			return opt;
	}
	return NULL;
}

static ArgParseResult parse_int_safe(
	const char *s,
	int *out)
{
	char *endptr;
	long val;

	if (s == NULL || *s == '\0')
		return ARGPARSE_ERR_INVALID_INT;
	val = strtol(s, &endptr, 10);
	if (*endptr != '\0')
		return ARGPARSE_ERR_INVALID_INT;
	*out = (int)val;
	return ARGPARSE_OK;
}

static ArgParseResult parse_double_safe(
	const char *s,
	double *out)
{
	char *endptr;
	double val;

	if (s == NULL || *s == '\0')
		return ARGPARSE_ERR_INVALID_DOUBLE;
	val = strtod(s, &endptr);
	if (*endptr != '\0')
		return ARGPARSE_ERR_INVALID_DOUBLE;
	*out = val;
	return ARGPARSE_OK;
}

static ArgParseResult assign_option_value(
	Option *opt,
	char *value_from_eq,
	int argc, char *argv[],
	int *i)
{
	int tmp_int;
	double tmp_double;
	const char *src;

	if (opt->type == ARGTYPE_FLAG)
	{
		*(byte *)opt->value = 1;
		return ARGPARSE_OK;
	}

	src = value_from_eq;
	if (src == NULL)
	{
		if (*i + 1 >= argc)
			return ARGPARSE_ERR_MISSING_VALUE;
		src = argv[++(*i)];
	}

	if (opt->type == ARGTYPE_INT)
	{
		ArgParseResult r = parse_int_safe(src, &tmp_int);
		if (r != ARGPARSE_OK)
			return r;
		*(int *)opt->value = tmp_int;
	}
	else if (opt->type == ARGTYPE_DOUBLE)
	{
		ArgParseResult r = parse_double_safe(src, &tmp_double);
		if (r != ARGPARSE_OK)
			return r;
		*(double *)opt->value = tmp_double;
	}
	else if (opt->type == ARGTYPE_STRING)
	{
		*(char **)opt->value = (char *)src;
	}
	return ARGPARSE_OK;
}

static ArgParseResult bind_positionals(ArgParser *parser)
{
	int defs = parser->positional_def_count;
	int args = parser->positional_count;

	/* Check required count */
	for (int i = 0; i < defs; ++i)
	{
		if (parser->positional_defs[i].required && i >= args)
			return (parser->last_error_arg = (char *)parser->positional_defs[i].name, parser->last_error = ARGPARSE_ERR_MISSING_VALUE);
	}
	/* Bind actual values */
	for (int i = 0; i < defs && i < args; ++i)
	{
		Positional *def = &parser->positional_defs[i];
		if (def->value)
			*(def->value) = parser->positional_args[i];
	}
	return parser->last_error = ARGPARSE_OK;
}
