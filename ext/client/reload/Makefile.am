INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libreload-cli.la

libreload_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libreload_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libreload_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-reload.c \
	ext-reload.h \
	ext-script.c
