INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libgenerator-cli.la

libgenerator_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libgenerator_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libgenerator_cli_la_SOURCES = \
	ext-dialog.c \
	ext-dialog.h \
	ext-generator.c \
	ext-generator.h \
	ext-match.c \
	ext-match.h \
	ext-module.c \
	ext-module.h \
	ext-rule.c \
	ext-rule.h \
	ext-script.c
