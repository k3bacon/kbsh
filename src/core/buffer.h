/*
 * Input buffer.
 * Copyright (C) 2011 Zack Parsons <k3bacon@gmail.com>
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

#ifndef BUFFER_H
#define BUFFER_H

#include <string.h>

struct Buffer {
	char *full;		/* full, raw buffer */
	size_t full_size;	/* size of malloc for .full in bytes */
	size_t full_used;	/* number bytes used in .full */
	char *pars;		/* after parsing, args separated by 0x1d */
	size_t pars_size;	/* size of malloc for .pars in bytes */
	size_t pars_used;	/* number of bytes used in .pars */
	char **word;		/* points to each argument in .pars */
	size_t word_size;	/* size of malloc for .word in (char *)s */
	size_t word_used;	/* number of (char *)s used in .word */
};

char *(*kbsh_buffer_gets_more)(void);

void kbsh_buffer_add_n_bytes(struct Buffer *b, size_t n);
void kbsh_buffer_reset(struct Buffer *b);

#endif/*BUFFER_H*/
