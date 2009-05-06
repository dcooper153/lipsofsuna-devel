INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libpackager-cli.la

libpackager_cli_la_LDFLAGS = -module -avoid-version
libpackager_cli_la_SOURCES = \
	ext-packager.c \
	ext-packager.h \
	ext-module.c \
	ext-module.h \
	ext-resources.c \
	ext-resources.h \
	ext-script.c
