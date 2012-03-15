/*
 * Buitin command: exit
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

#include "core/kbsh.h"
#include "builtin/builtin.h"

static int builtin_exit(int argc, char **argv)
{
	int status = 0;
	if (!argv || argc < 2)
		goto end;
	status = atoi(argv[1]);
end:
	kbsh_exit(status);
	return 1;
}

struct Builtin bi_exit = {
	"exit",
	builtin_exit
};
