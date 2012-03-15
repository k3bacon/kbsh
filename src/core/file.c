/*
 * Read script file.
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

static char **line;

static struct Buffer buffer;

static char *kbsh_fpars_gets_more(void)
{
	char **temp;
	temp = malloc(sizeof(*temp));
	if (!temp)
		kbsh_exit(errno);
	*temp = malloc(sizeof(**temp) * (kfile.line_read_length));
	if (!*temp)
		kbsh_exit(errno);
	char *out = NULL;

	while (1) {
		if (getline(temp, &kfile.line_read_length, kfile.file) == -1) {
			kfile.line_number++;
			if (*temp)
				free(*temp);
			if (temp)
				free(temp);
			return NULL;
		}
		if (**line == '#')
			continue;
		else
			break;
	}
	out = *temp;
	free(temp);
	return out;
}

void kbsh_fpars_exit(void)
{
	if (*line)
		free(*line);
	if (line)
		free(line);
	if (kfile.file)
		fclose(kfile.file);
	kbsh_buffer_reset(&buffer);
}

void kbsh_fpars_init(char *fname_in)
{
	kbsh_clean = kbsh_fpars_exit;
	kbsh_mode = FILE_M;
	kbsh_buffer_gets_more = kbsh_fpars_gets_more;
	kfile.line_read_length = LINE_READ_LENGTH;
	kfile.line_number = 0;
	if (!fname_in) {
		kfile.name = "stdin";
		kfile.file = stdin;
	} else {
		kfile.name = fname_in;
		kfile.file = fopen(kfile.name, "r");
		if (!kfile.file) {
			fputs(_("kbsh: file not found: "), stdout);
			puts(kfile.name);
			kbsh_exit(errno);
		}
		program_name = kfile.name;
	}
	line = malloc(sizeof(*line));
	if (!line)
		kbsh_exit(errno);
	*line = malloc(sizeof(**line) * (kfile.line_read_length));
	if (!*line)
		kbsh_exit(errno);
}

void kbsh_fpars_main(void)
{
	while (getline(line, &kfile.line_read_length, kfile.file) != -1) {
		kfile.line_number++;
		if (**line == '#')
			continue;
		buffer.full = *line;

		kbsh_parse(&buffer);
		if (parse_err)
			kbsh_exit(parse_err);

		kbsh_main((int)buffer.word_used, buffer.word);
		kbsh_buffer_reset(&buffer);

		*line = NULL;
		*line = malloc(sizeof(**line) * (kfile.line_read_length));
		if (!*line)
			kbsh_exit(errno);
	}
}
