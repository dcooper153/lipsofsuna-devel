INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libevents-cli.la

libevents_cli_la_LDFLAGS = -module -avoid-version
libevents_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
