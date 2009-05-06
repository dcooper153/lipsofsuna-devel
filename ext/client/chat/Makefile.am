INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libchat-cli.la

libchat_cli_la_LDFLAGS = -module -avoid-version
libchat_cli_la_SOURCES = \
	ext-entry.c \
	ext-entry.h \
	ext-history.c \
	ext-history.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
