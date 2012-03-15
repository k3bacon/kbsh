/*
 * Read commands from "-c [string]".
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

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "core/kbsh.h"
#include "core/buffer.h"
#include "core/parse.h"

static char *string;
static struct Buffer buffer;

static char *kbsh_cmdstr_gets()
{
	char *line = NULL;
	size_t index = 0;

	if (string == NULL)
		return NULL;

	while (string[index] != '\n' && string[index] != '\0')
		index++;

	line = malloc(sizeof(*line) * (index));
	if (!line)
		kbsh_exit(errno);
	strncpy(line, string, index);
	strcat(line, "\n");

	if (string[index] == '\n' && string[index + 1] != '\0')
		string = &string[index + 1];
	else
		string = NULL;
	return line;
}

static char *kbsh_cmdstr_gets_more(void)
{
	char *temp = NULL;
	while (1) {
		if (temp) {
			free(temp);
			temp = NULL;
		}
		temp = kbsh_cmdstr_gets();
		if (!temp)
			return temp;
		if (*temp == '#')
			continue;
		else
			break;
	}
	return temp;
}

void kbsh_cmdstr_exit(void)
{
	return;
}

void kbsh_cmdstr_init(char *str)
{
	if (str == NULL)
		kbsh_exit(EINVAL);
	kbsh_clean = kbsh_cmdstr_exit;
	kbsh_buffer_gets_more = kbsh_cmdstr_gets_more;
	kbsh_mode = STRG_M;
	string = str;
}

void kbsh_cmdstr_main(void)
{
	while (1) {
		while (1) {
			if (buffer.full) {
				free(buffer.full);
				buffer.full = NULL;
			}
			buffer.full = kbsh_cmdstr_gets();
			if (!buffer.full)
				kbsh_exit(0);
			if (buffer.full && *buffer.full) {
				if (*buffer.full == '#')
					continue;
				break;
			}
		}
		kbsh_parse(&buffer);
		if (parse_err)
			kbsh_exit(parse_err);
		kbsh_main((int)buffer.word_used, buffer.word);
		kbsh_buffer_reset(&buffer);
	}
}
