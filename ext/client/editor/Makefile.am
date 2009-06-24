INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libeditor-cli.la

libeditor_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libeditor_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libeditor_cli_la_SOURCES = \
	ext-dialog.c \
	ext-dialog.h \
	ext-editor.c \
	ext-editor.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
