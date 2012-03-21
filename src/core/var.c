/*
 * Manage variables.
 * Copyright (C) 2012 Zack Parsons <k3bacon@gmail.com>
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
#include <string.h>
#include <errno.h>

#include "core/kbsh.h"
#include "core/var.h"

static struct Var {
	char *name;
	char *val;
} *var;

static size_t var_stack_size;

static struct Var *kbsh_var_createvar(char *name, char *val)
{
	size_t varss = var_stack_size;
	if (!name) {
		errno = EINVAL;
		return NULL;
	}

	var = realloc(var, (sizeof(struct Var)) * (varss + 1));
	if (!var)
		kbsh_exit(errno);

	var[varss - 1].name = name;
	var[varss - 1].val = val;
	var[varss].name = NULL;
	var[varss].val = NULL;

	++var_stack_size;

	return &var[varss];
}

static struct Var *kbsh_var_getvar(const char *name)
{
	size_t ind = 0;

	if (!name || !var) {
		errno = EINVAL;
		return NULL;
	}

	while (var[ind].name) {
		if (!strcmp(var[ind].name, name))
			return &var[ind];
		++ind;
	}

	return NULL;
}

void kbsh_var_init(char **env)
{
	size_t ind = 0;
	var_stack_size = 1;

	var = malloc(sizeof(struct Var));
	if (!var)
		kbsh_exit(errno);

	var[0].name = NULL;
	var[0].val = NULL;

	while (env[ind] != NULL) {
		kbsh_var_setvar(env[ind], 1);
		++ind;
	}
}

void kbsh_var_exit(void)
{
	size_t ind = 0;

	if (!var)
		return;

	while (var[ind].name != NULL) {
		free(var[ind].name);
		free(var[ind].val);
		++ind;
	}
	free(var);
}

char *kbsh_var_getval(const char *name)
{
	if (!name || !var)
		return NULL;

	size_t ind = 0;

	while (var[ind].name != NULL) {
		if (!strcmp(var[ind].name, name))
			return var[ind].val;
		++ind;
	}

	return "";
}

int kbsh_var_setval(const char *name, const char *val, int ovrwrite)
{
	struct Var *varp = NULL;
	char *name_out = NULL;
	char *val_out = NULL;
	size_t ind = 0;
	size_t size = 0;
	int ret = 0;

	if (!name || !val || !var)
		return -EINVAL;

	varp = kbsh_var_getvar(name);
	if (varp && !ovrwrite)
		return -EPERM;

	size = strlen(name) + 1;
	name_out = malloc((sizeof(char)) * size);
	if (!name_out)
		kbsh_exit(errno);

	while (name[ind]) {
		name_out[ind] = name[ind];
		++ind;
	}
	name_out[ind] = '\0';
	ind = 0;

	if (!val)
		goto skip_val;

	size = strlen(val) + 1;
	val_out = malloc((sizeof(char)) * size);
	if (!val_out) {
		free(name_out);
		kbsh_exit(errno);
	}

	while (val[ind]) {
		val_out[ind] = val[ind];
		++ind;
	}
	val_out[ind] = '\0';
	ind = 0;

skip_val:
	if (!varp) {
		if (!kbsh_var_createvar(name_out, val_out))
			ret = errno * -1;
	} else {
		free(name_out);
		free(varp->val);
		varp->val = val_out;
		ret = 0;
	}

	return ret;
}

int kbsh_var_setvar(const char *var_str, int ovrwrite)
{
	size_t ind0 = 0;
	size_t ind1 = 0;
	size_t size = 0;
	char *name = NULL;
	char *val = NULL;
	int ret = 0;

	if (!var || !var_str)
		return -EINVAL;

	while (var_str[ind0] && var_str[ind0] != '=') {
		++ind0;
	}
	size = ind0 + 1;
	ind0 = 0;

	name = malloc((sizeof(char) * size));
	if (!name)
		kbsh_exit(errno);

	while (var_str[ind0] && var_str[ind0] != '=') {
		name[ind0] = var_str[ind0];
		++ind0;
	}
	name[ind0] = '\0';
	if (!var_str[ind0]) {
		goto skip_val;
	}

	ind1 = ++ind0;

	while (var_str[ind0]) {
		++ind0;
	}
	size = (ind0 - ind1) + 1;
	ind0 = 0;

	val = malloc((sizeof(char)) * size);
	if (!val) {
		free(name);
		kbsh_exit(errno);
	}

	while (var_str[ind1]) {
		val[ind0] = var_str[ind1];
		++ind0;
		++ind1;
	}
	val[ind0] = '\0';

skip_val:
	ret = kbsh_var_setval(name, val, ovrwrite);
	free(name);
	free(val);

	return ret;
}

int kbsh_var_export(const char *name)
{
	struct Var *varp = NULL;
	if (!name)
		return -EINVAL;

	varp = kbsh_var_getvar(name);
	if (!varp)
		return errno * -1;

	return (setenv(var->name, var->val, 1) * -1);
}
