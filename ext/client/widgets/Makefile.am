INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libwidgets-cli.la

libwidgets_cli_la_LDFLAGS = -module -avoid-version
libwidgets_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
