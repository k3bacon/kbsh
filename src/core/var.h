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

#ifndef VAR_H
#define VAR_H

#include <unistd.h>

void kbsh_var_init(char **env);
void kbsh_var_exit(void);
char *kbsh_var_getval(const char *name);
int kbsh_var_setval(const char *name, const char *val, int ovrwrite);
int kbsh_var_setvar(const char *var_str, int ovrwrite);
int kbsh_var_export(const char *name);

#endif/*VAR_H*/
