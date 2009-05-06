INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libeditor-cli.la

libeditor_cli_la_LDFLAGS = -module -avoid-version
libeditor_cli_la_SOURCES = \
	ext-dialog.c \
	ext-dialog.h \
	ext-editor.c \
	ext-editor.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
