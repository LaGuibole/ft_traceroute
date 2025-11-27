/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guphilip <guphilip@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 13:05:41 by guphilip          #+#    #+#             */
/*   Updated: 2025/11/27 13:05:52 by guphilip         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <stddef.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>

# define MAX_OPTIONS     32
# define MAX_POSITIONAL  32

typedef unsigned char   byte;

typedef enum e_argtype
{
	ARGTYPE_FLAG,
	ARGTYPE_INT,
	ARGTYPE_DOUBLE,
	ARGTYPE_STRING
}   ArgType;

typedef struct s_option
{
	char        *short_flag;
	char        *long_flag;
	ArgType     type;
	void        *value;
	char        *description;
}   Option;

typedef struct s_positional
{
	const char  *name;
	char        **value;
	const char  *description;
	int         required;
}   Positional;


typedef enum e_argparse_result
{
	ARGPARSE_OK = 0,
	ARGPARSE_ERR_TOO_MANY_OPTIONS,
	ARGPARSE_ERR_TOO_MANY_POSITIONALS,
	ARGPARSE_ERR_MISSING_VALUE,
	ARGPARSE_ERR_INVALID_INT,
	ARGPARSE_ERR_INVALID_DOUBLE,
	ARGPARSE_ERR_ALLOC,
	ARGPARSE_ERR_UNKNOWN_OPTION
}   ArgParseResult;

typedef struct s_argparser
{
	char        	*program_name;
		
	Option      	options[MAX_OPTIONS];
	int         	option_count;
	
	char        	*positional_args[MAX_POSITIONAL];
	int         	positional_count;
	
	Positional  	positional_defs[MAX_POSITIONAL];
	int         	positional_def_count;

	ArgParseResult	last_error;
	char			*last_error_arg;
}   ArgParser;

/* Core API */
void            init_arg_parser(ArgParser *parser, const char *program_name);
ArgParseResult  add_option(ArgParser *parser, const Option *opt_def);
ArgParseResult  parse_arguments(ArgParser *parser, int argc, char *argv[]);
void            print_usage(const ArgParser *parser);
void            free_arg_parser(ArgParser *parser);
ArgParseResult	print_argparse_error(ArgParser *parser);
ArgParseResult	add_options(ArgParser *parser, const Option *opts, int count);

/* Option builders */

Option          build_option(
					const char *short_flag,
					const char *long_flag,
					ArgType type,
					void *value,
					const char *description);

/* Flags: underlying value is a byte (0/1) */
Option          build_flag_option(
					const char *short_flag,
					const char *long_flag,
					byte *value,
					const char *description);

/* Integer option: underlying value is an int */
Option          build_int_option(
					const char *short_flag,
					const char *long_flag,
					int *value,
					const char *description);

/* String option: underlying value is a char* variable (pass &my_char_ptr) */
Option          build_str_option(
					const char *short_flag,
					const char *long_flag,
					char **value,
					const char *description);

/* Double option: underlying value is a double */
Option          build_double_option(
					const char *short_flag,
					const char *long_flag,
					double *value,
					const char *description);

/* Positional argument registration */

ArgParseResult  add_positional(
					ArgParser *parser,
					const char *name,
					char **value,
					const char *description,
					int required);

#endif