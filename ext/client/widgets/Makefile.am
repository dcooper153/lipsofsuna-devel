INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libwidgets-cli.la

libwidgets_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libwidgets_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libwidgets_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
