/*
 * Change directory.
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
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include "core/kbsh.h"
#include "core/buffer.h"
#include "core/env.h"
#include "builtin/builtin.h"

int kbsh_builtin_cd(struct Buffer *b)
{
	if (!b)
		kbsh_exit(EINVAL);

	size_t cd_size = 1;
	size_t up = 1;
	char *cd  = NULL;

	if (!b->word_used || !b->word[0])
		goto end;
	if (b->word_used < 2) {
		if (chdir(env.home))
			perror("kbsh: cd");
		else
			goto done;
		goto end;
	}
	cd_size += 1 + strlen(b->word[up]);
	cd = malloc(sizeof(*cd) * cd_size);
	if (!cd)
		kbsh_exit(errno);
	strcpy(cd, b->word[up]); up++;
	while (b->word[up]) {
		cd_size += 1 + strlen(b->word[up]);
		cd = realloc(cd, sizeof(*cd) * cd_size);
		if (!cd)
			kbsh_exit(errno);
		strcat(cd, " ");
		strcat(cd, b->word[up]);
		up++;
	}
	if (chdir(cd))/*chdir*/
		perror("kbsh: cd");
	else {
done:
		free(env.cwd);
		env.cwd = getcwd(NULL, 0);
		if (setenv("PWD", env.cwd, 1))
			perror("kbsh");
	}
end:
	free(cd);
	return 0;
}

struct Builtin bi_cd = {
	"cd",
	kbsh_builtin_cd
};
