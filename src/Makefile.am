bin_PROGRAMS = kbsh

kbsh_core_dir = core
kbsh_builtin_dir = builtin

kbsh_core_sources =\
$(kbsh_core_dir)/buffer.c\
$(kbsh_core_dir)/cmdstr.c\
$(kbsh_core_dir)/env.c\
$(kbsh_core_dir)/file.c\
$(kbsh_core_dir)/input.c\
$(kbsh_core_dir)/kbsh.c\
$(kbsh_core_dir)/main.c\
$(kbsh_core_dir)/parse.c\
$(kbsh_core_dir)/prompt.c\
$(kbsh_core_dir)/sig.c

kbsh_builtin_sources =\
$(kbsh_builtin_dir)/builtin.c\
$(kbsh_builtin_dir)/cd.c\
$(kbsh_builtin_dir)/exit.c

kbsh_core_headers =\
$(kbsh_core_dir)/buffer.h\
$(kbsh_core_dir)/cmdstr.h\
$(kbsh_core_dir)/env.h\
$(kbsh_core_dir)/file.h\
$(kbsh_core_dir)/input.h\
$(kbsh_core_dir)/kbsh.h\
$(kbsh_core_dir)/parse.h\
$(kbsh_core_dir)/prompt.h\
$(kbsh_core_dir)/sig.h

kbsh_builtin_headers =\
$(kbsh_builtin_dir)/builtin.h

kbsh_SOURCES =\
$(kbsh_core_sources)\
$(kbsh_builtin_sources)\
$(kbsh_core_headers)\
$(kbsh_builtin_headers)\
localize.h

kbsh_LDADD = @LIBINTL@ $(top_builddir)/lib/gnulib/libgnu.a

kbsh_CFLAGS =\
-Wall -Werror -Wextra

AM_CPPFLAGS =\
-I$(top_builddir)/lib/gnulib\
-I$(top_builddir)/src
