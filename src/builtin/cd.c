/*
 * Builtin command: cd
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
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "core/kbsh.h"
#include "core/var.h"
#include "builtin/builtin.h"

static int builtin_cd(int argc, char **argv)
{
	size_t cd_size = 1;
	size_t up = 1;
	char *cd = NULL;
	char *temp = NULL;

	if (!argc || !argv || !argv[0])
		goto end;

	if (argc < 2) {
		if (chdir(kbsh_var_getval("HOME")))
			perror("kbsh: cd");
		else
			goto done;
		goto end;
	}
	cd_size += 1 + strlen(argv[up]);
	cd = malloc(sizeof(*cd) * cd_size);
	if (!cd)
		kbsh_exit(errno);
	strcpy(cd, argv[up++]);
	while (argv[up]) {
		cd_size += 1 + strlen(argv[up]);
		cd = realloc(cd, sizeof(*cd) * cd_size);
		if (!cd)
			kbsh_exit(errno);
		strcat(cd, " ");
		strcat(cd, argv[up]);
		up++;
	}
	if (chdir(cd))
		perror("kbsh: cd");
	else {
done:
		temp = getcwd(NULL, 0);
		kbsh_var_setval("PWD", temp, 1);
		kbsh_var_export("PWD");
		free(temp);
	}
end:
	free(cd);

	return 0;
}

struct Builtin bi_cd = {
	"cd",
	builtin_cd
};
