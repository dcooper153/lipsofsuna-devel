INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libgenerator-cli.la

libgenerator_cli_la_LDFLAGS = -module -avoid-version
libgenerator_cli_la_SOURCES = \
	ext-dialog.c \
	ext-dialog.h \
	ext-generator.c \
	ext-generator.h \
	ext-match.c \
	ext-match.h \
	ext-module.c \
	ext-module.h \
	ext-rule.c \
	ext-rule.h \
	ext-script.c
