INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libchat-cli.la

libchat_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libchat_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libchat_cli_la_SOURCES = \
	ext-history.c \
	ext-history.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
