INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libcamera-cli.la

libcamera_cli_la_LDFLAGS = -module -avoid-version
libcamera_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
