/*
 * Kbsh core.
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

#ifndef KBSH_H
#define KBSH_H

#include "core/buffer.h"

char *program_name;

struct Kbsh_options {
	int c;
	char *c_arg;
	int i;
	int s;
} kbsh_options;

enum Mode {
	FILE_M,
	INTR_M,
	STRG_M
} kbsh_mode;

int stdin_isatty;
int stdout_isatty;
int stderr_isatty;

void (*kbsh_clean)(void);

void kbsh_exit(int exit_status);
void kbsh_init(void);
void kbsh_main(struct Buffer *b);

#endif/*KBSH_H*/
