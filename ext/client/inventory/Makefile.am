INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libinventory-cli.la

libinventory_cli_la_LDFLAGS = -module -avoid-version
libinventory_cli_la_SOURCES = \
	ext-inventory.c \
	ext-inventory.h \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-widget.c \
	ext-widget.h
