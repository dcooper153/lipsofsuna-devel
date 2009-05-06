INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	liboptions-cli.la

liboptions_cli_la_LDFLAGS = -module -avoid-version
liboptions_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-options.c \
	ext-options.h \
	ext-script.c
