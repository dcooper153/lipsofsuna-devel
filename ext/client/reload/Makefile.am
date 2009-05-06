INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libreload-cli.la

libreload_cli_la_LDFLAGS = -module -avoid-version
libreload_cli_la_SOURCES = \
	ext-blender.c \
	ext-gimp.c \
	ext-image.c \
	ext-module.c \
	ext-module.h \
	ext-reload.c \
	ext-reload.h \
	ext-script.c
