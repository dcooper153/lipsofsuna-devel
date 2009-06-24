INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libslots-cli.la

libslots_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libslots_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libslots_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-slot.c \
	ext-slot.h \
	ext-slots.c \
	ext-slots.h
