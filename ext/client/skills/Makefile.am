INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libskills-cli.la

libskills_cli_la_LDFLAGS = -module -avoid-version
libskills_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-skills.c \
	ext-skills.h \
	ext-widget.c \
	ext-widget.h
