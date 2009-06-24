INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libskills-cli.la

libskills_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libskills_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libskills_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-skills.c \
	ext-skills.h \
	ext-widget.c \
	ext-widget.h
