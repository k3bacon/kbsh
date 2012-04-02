/*
 * The K3BACON Shell (kbsh).
 * Copyright (C) 2011, 2012 Zack Parsons <k3bacon@gmail.com>
 *
 * This file is part of kbsh.
 *
 * Kbsh is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kbsh is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kbsh.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <getopt.h>

#include "localize.h"

#include "core/kbsh.h"
#include "core/cmdstr.h"
#include "core/file.h"
#include "core/input.h"

#ifdef NO_MAIN_ENV_ARG
extern char **environ;
#endif

static int print_help_flag;

#define SOPTS "c:isabefhkmnptuvxBCHP"
static const struct option longopts[] = {
	{ "help", no_argument, &print_help_flag, 1 },
	{ "version", no_argument, &print_help_flag, 2 },
	{ NULL, required_argument, NULL, 'c' },
	{ NULL, no_argument, NULL, 'i' },
	{ NULL, no_argument, NULL, 's' },
	{ NULL, 0, NULL, 0 }
};

static void print_help(void);
static void print_version(void);

int main(int argc, char **argv
#ifndef NO_MAIN_ENV_ARG
	 , char **envp
#endif/*NO_MAIN_ENV_ARG*/
)
{
	program_name = argv[0];
	int optc;
	int lose = 0;

	setlocale(LC_ALL, "");

#if ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif/*ENABLE_NLS*/

	stdin_isatty = isatty(STDIN_FILENO);
	stdout_isatty = isatty(STDIN_FILENO);
	stderr_isatty = isatty(STDERR_FILENO);

	while ((optc = getopt_long(argc, argv, SOPTS, longopts, NULL)) != -1) {
		switch (optc) {
		case 'c': /* command string */
			kbsh_options.c = 1;
			kbsh_options.c_arg = optarg;
			break;
		case 'i': /* interactive shell */
			kbsh_options.i = 1;
			break;
		case 's': /* default: read from standard input */
			kbsh_options.s = 1;
			break;
		case 'a':
		case 'b':
		case 'e':
		case 'f':
		case 'h':
		case 'k':
		case 'm':
		case 'n':
		case 'p':
		case 't':
		case 'u':
		case 'v':
		case 'x':
		case 'B':
		case 'C':
		case 'H':
		case 'P':
			break;
		default:
			lose = 1;
			break;
		}
	}

	switch (print_help_flag) {
	case 1:
		print_help();
		exit(0);
		break;
	case 2:
		print_version();
		exit(0);
		break;
	default:
		break;
	}

	if (lose) {
		fprintf(stderr, "Try \"%s --help\" for more information\n",
			program_name);
		exit(1);
	}

	kbsh_init(
#ifdef NO_MAIN_ENV_ARG
		  environ
#else
		  envp
#endif/*NO_MAIN_ENV_ARG*/
	);

	if (optind < argc) {
		/* File mode */
		/* Read from a file */
		kbsh_fpars_init(argv[optind]);
		kbsh_fpars_main();
	} else if ((!stdin_isatty || !stderr_isatty) &&
		   (!kbsh_options.i && !kbsh_options.c)) {
		/* Pipe mode */
		/* stdin piped, NULL tells kbsh_fpars to read from stdin */
		kbsh_fpars_init(NULL);
		kbsh_fpars_main();
	} else if (kbsh_options.c) {
		/* Command string mode */
		/* Read command from "-c [string]" */
		kbsh_cmdstr_init(kbsh_options.c_arg);
		kbsh_cmdstr_main();
	} else {
		/* Interactive mode */
		/* Read input from user */
		kbsh_input_init();
		kbsh_input_main();
	}

	kbsh_exit(0);
	return 0;
}

static void print_help(void)
{
	printf(
"K3BACON Shell (%s) %s\n", PACKAGE, VERSION);
	printf(_(
"Usage: %s [Long options] [Shell options] [File]\n"), program_name);
	puts("");
	puts(_(
"Long options:\n"
"  --help                Print this help text and exit\n"
"  --version             Print version text and exit"));
	puts("");
	puts(_(
"Shell options:\n"
"  -c [string]           Read commands from [string]\n"
"  -i                    Run kbsh as an interactive shell\n"
"  -s                    Read commands from standard input\n"
"  -abefhkmnptuvxBCHP    Only used for compatibility"));
	puts("");
	puts(_(
"File:\n"
"  Read commands from a plain-text file and exit"));
	puts("");
	printf(_(
"Report bugs to: %s\n"), PACKAGE_BUGREPORT);
#ifdef PACKAGE_PACKAGER_BUG_REPORTS
	printf(_(
"Report %s bugs to: %s\n"), PACKAGE_PACKAGER, PACKAGE_PACKAGER_BUG_REPORTS);
#endif
	printf(_(
"\%s home page: %s\n"), PACKAGE_NAME, "<https://github.com/k3bacon/kbsh/>");
}

static void print_version(void)
{
	printf(
"K3BACON Shell (%s) %s\n", PACKAGE, VERSION);
	puts("");
	puts(
"Copyright (C) 2012 Zack Parsons\n"
"License: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
"This is free software; you are free to change and redistribute it.\n"
"This program has NO WARRANTY, to the extent permitted by law");
}
