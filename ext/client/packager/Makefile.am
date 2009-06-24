INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libpackager-cli.la

libpackager_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libpackager_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libpackager_cli_la_SOURCES = \
	ext-packager.c \
	ext-packager.h \
	ext-module.c \
	ext-module.h \
	ext-resources.c \
	ext-resources.h \
	ext-script.c
