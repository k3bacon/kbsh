/*
 * Find builtin commands.
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

#include <string.h>
#include <errno.h>

#include "core/kbsh.h"
#include "builtin/builtin.h"

int kbsh_run_builtin(int argc, char **argv)
{
	if (!argc || !argv) {
		return -EINVAL;
	}

	if (!strcmp(argv[0], bi_cd.call_str)) {
		return bi_cd.init(argc, argv);
	} else if (!strcmp(argv[0], bi_exit.call_str)) {
		return bi_exit.init(argc, argv);
	}

	return BUILTIN_NOT_FOUND;
}
