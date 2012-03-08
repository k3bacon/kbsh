/*
 * Find builtin commands.
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

#include <string.h>
#include <errno.h>

#include "core/kbsh.h"
#include "core/buffer.h"
#include "builtin/builtin.h"

int kbsh_find_builtin(struct Buffer *b)
{
	if (!b)
		kbsh_exit(EINVAL);
	/* find builtin commands */
	if (!b->word || !*b->word)/* empty input */
		goto found;

	if (!strcmp(b->word[0], bi_cd.command)) {
		bi_cd.init(b);
		goto found;
	} else if (!strcmp(b->word[0], bi_exit.command)) {
		bi_exit.init(b);
		goto found;
	}

	return 0; /* Continue and find external commands */
found:
	return 1; /* Builtin command found; Don't continue */
}
