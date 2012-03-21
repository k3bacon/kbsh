/*
 * Parse input buffer.
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
#include <errno.h>

#include "localize.h"

#include "core/kbsh.h"
#include "core/buffer.h"
#include "core/file.h"
#include "core/parse.h"
#include "core/var.h"

static struct Buffer *buffer;

static struct Syntax_Err {
	int checkme;
	enum Syntax_Error_Type type;
} syntax_err;

static size_t argno;
static size_t bfind;
static size_t bpind;
static size_t hmind;
static size_t loop;

static int ignore_next;
static int in_arg;
static int in_quote;
static int in_squote;
static int in_dquote;

static const char *kbsh_get_syntax_err_msg(void)
{
	switch (syntax_err.type) {
	case MISSING_SINGLE_QUOTE:
		return _("missing \'");
		break;
	case MISSING_DOUBLE_QUOTE:
		return _("missing \"");
		break;
	case UNEXPECTED_EOF:
		return _("unexpected EOF");
	default:
		return _("unknown error");
		break;
	}
	return NULL;
}

static void kbsh_parse_gets_more(void)
{
	char *temp = NULL;
	size_t temp_size = 0;
	temp = kbsh_buffer_gets_more();
	if (!temp) {
		syntax_err.checkme = 1;
		syntax_err.type = UNEXPECTED_EOF;
		loop = 0;
		return;
	}
	temp_size = strlen(temp);
	if (temp_size) {
		temp_size += 1;
		if (!kbsh_buffer_add_bytes(buffer, temp_size))
			kbsh_exit(errno);
		strcat(buffer->full, temp);
	}
	free(temp);
}

static void kbsh_parse_tok(void)
{
	size_t ind = 0;
	char *temp = NULL;

	buffer->word_size = ++argno;

	if (buffer->word)
		free(buffer->word);
	buffer->word = malloc(sizeof(*buffer->word) * (buffer->word_size));
	if (!buffer->word)
		kbsh_exit(errno);

	temp = strtok(buffer->pars, "\x1d");
	while (temp != NULL) {
		buffer->word[ind] = temp;
		temp = strtok(NULL, "\x1d");
		ind++;
	}
	buffer->word[ind] = NULL;
	buffer->word_used = ind;
}

static void parse_newline(void)
{
	if (ignore_next || in_quote)
		kbsh_parse_gets_more();

	if (!ignore_next && in_quote) {
		buffer->pars[bpind] = buffer->full[bfind];
		bpind++;
		if (!in_arg) {
			in_arg = 1;
			argno++;
		}
	}
	ignore_next = 0;
}

static void parse_space(void)
{
	if (ignore_next || in_quote) {
		ignore_next = 0;
		if (!in_arg) {
			in_arg = 1;
			argno++;
		}
		buffer->pars[bpind] = buffer->full[bfind];
	} else {
		in_arg = 0;
		/* Separate arguments */
		buffer->pars[bpind] = 0x1d;
	}
	bpind++;
}

static void parse_dquote(void)
{
	if (ignore_next || in_squote) {
		ignore_next = 0;
		buffer->pars[bpind] = buffer->full[bfind];
		bpind++;
		if (!in_arg) {
			in_arg = 1;
			argno++;
		}
	} else if (in_dquote) {
		syntax_err.checkme = 0;
		in_dquote = 0;
		in_quote = 0;
	} else {
		syntax_err.checkme = 1;
		syntax_err.type = MISSING_DOUBLE_QUOTE;
		in_dquote = 1;
		in_quote = 1;
	}
}

static void parse_squote(void)
{
	if (ignore_next || in_dquote) {
		ignore_next = 0;
		buffer->pars[bpind] = buffer->full[bfind];
		bpind++;
		if (!in_arg) {
			in_arg = 1;
			argno++;
		}
	} else if (in_squote) {
		syntax_err.checkme = 0;
		in_squote = 0;
		in_quote = 0;
	} else {
		syntax_err.checkme = 1;
		syntax_err.type = MISSING_SINGLE_QUOTE;
		in_squote = 1;
		in_quote = 1;
	}
}

static void parse_bslash(void)
{
	if (ignore_next) {
		ignore_next = 0;
		buffer->pars[bpind] = buffer->full[bfind];
		bpind++;
		if (!in_arg) {
			in_arg = 1;
			argno++;
		}
	} else if (in_squote) {
		buffer->pars[bpind] = buffer->full[bfind];
		bpind++;
	} else
		ignore_next = 1;
}

static void parse_tilde(void)
{
	if (ignore_next || in_quote || in_arg) {
		ignore_next = 0;
		buffer->pars[bpind] = buffer->full[bfind];
	} else {
		char *home_dir = kbsh_var_getval("HOME");
		if (!home_dir)
			goto end;

		buffer->pars_size += strlen(home_dir);
		buffer->pars = realloc(buffer->pars, sizeof(*buffer->pars)
				       * (buffer->pars_size));
		if (!buffer->pars)
			kbsh_exit(errno);
		buffer->pars =  buffer->pars;

		while (home_dir[hmind] != '\0')
			buffer->pars[bpind++] = home_dir[hmind++];

		bpind--;
		hmind = 0;
	}
end:
	if (!in_arg) {
		in_arg = 1;
		argno++;
	}
	bpind++;
}

static void parse_default(void)
{
	if (!in_arg) {
		in_arg = 1;
		argno++;
	}
	ignore_next = 0;
	buffer->pars[bpind] = buffer->full[bfind];
	bpind++;
}

void kbsh_parse(struct Buffer *b)
{
	if (!b)
		kbsh_exit(EINVAL);
	buffer = b;
	argno = 0;
	bfind = 0;
	bpind = 0;
	hmind = 0;

	loop = 1;
	ignore_next = 0;
	in_arg = 0;
	in_quote = 0;
	in_squote = 0;
	in_dquote = 0;

	syntax_err.checkme = 0;
	syntax_err.type = NO_SERROR;

	buffer->full_size = strlen(buffer->full);
	buffer->pars_size = buffer->full_size;

	if (buffer->pars)
		free(buffer->pars);
	buffer->pars = malloc(sizeof(*buffer->pars) * (buffer->pars_size));
	if (!buffer->pars)
		kbsh_exit(errno);

	while (1) {
		switch (buffer->full[bfind]) {
		case '\0':
			loop = 0;
			break;

		case '\n':
			parse_newline();
			break;

		case '\t':
		case ' ':
			parse_space();
			break;

		case '\"':
			parse_dquote();
			break;

		case '\'':
			parse_squote();
			break;

		case '\\':
			parse_bslash();
			break;

		case '~':
			parse_tilde();
			break;

		default:
			parse_default();
			break;
		}
		if (loop)
			bfind++;
		else
			break;
	}

	buffer->pars[bpind] = '\0';

	if (syntax_err.checkme) {
		fprintf(stderr, "%s: ", program_name);
		if (kbsh_mode == FILE_M)
			fprintf(stderr, _("line %Zu: "), kfile.line_number);
		fprintf(stderr, _("syntax error: "));
		fprintf(stderr, "%s\n", kbsh_get_syntax_err_msg());
		parse_err = syntax_err.type;
	}
	kbsh_parse_tok();
}
