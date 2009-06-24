INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = liboptions-cli.la

liboptions_cli_la_LDFLAGS = -module -avoid-version -no-undefined
liboptions_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
liboptions_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-options.c \
	ext-options.h \
	ext-script.c
