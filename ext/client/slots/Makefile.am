INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libslots-cli.la

libslots_cli_la_LDFLAGS = -module -avoid-version
libslots_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-slot.c \
	ext-slot.h \
	ext-slots.c \
	ext-slots.h
