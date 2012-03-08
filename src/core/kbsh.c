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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>

#include "localize.h"

#include "core/kbsh.h"
#include "core/buffer.h"
#include "core/env.h"
#include "core/sig.h"
#include "builtin/builtin.h"

static int kbsh_exec(char **argums);
static void kbsh_fork(struct Buffer *b);

void kbsh_init(void)
{
	kbsh_sig_init();
	kbsh_env_init();
}

void kbsh_exit(int exit_status)
{
	if (kbsh_clean)
		kbsh_clean();
	kbsh_env_exit();
	exit(exit_status);
}

void kbsh_main(struct Buffer *b)
{
	if (!kbsh_find_builtin(b))
		kbsh_fork(b);
	kbsh_env_update();
}

static int kbsh_exec(char **argums)
{
	int err = 0;

	err = execvp(argums[0], argums);
	if (err) {
		fprintf(stderr, "%s: ", program_name);
		perror(argums[0]);
	}
	return err;
}

static void kbsh_fork(struct Buffer *b)
{
	pid_t pid = fork();

	if (!pid)
		_exit(kbsh_exec(b->word));
	else if (pid > 0)
		wait(NULL);
	if (pid < 0)
		kbsh_exit(errno);
}
