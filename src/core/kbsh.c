/*
 * Kbsh core.
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

#include <unistd.h>
#include <sys/wait.h>

#include "localize.h"

#include "core/kbsh.h"
#include "core/buffer.h"
#include "core/env.h"
#include "core/sig.h"
#include "builtin/builtin.h"

static int kbsh_exec(int argc, char **argv)
{
	volatile int err = -1;
	pid_t pid = 0;

	if (!argc)
		return -EINVAL;

	pid = fork();

	if (!pid) {
		err = execvp(argv[0], argv);
		if (err) {
			fprintf(stderr, "%s: ", program_name);
			perror(argv[0]);
		}
		_exit(err);
	} else if (pid > 0)
		wait(NULL);
	if (pid < 0)
		kbsh_exit(errno);

	return (int)err;
}

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

int kbsh_main(int argc, char **argv)
{
	int ret = 0;

	if ((ret = kbsh_run_builtin(argc, argv)) == BUILTIN_NOT_FOUND)
		ret = kbsh_exec(argc, argv);
	kbsh_env_update();

	return ret;
}
