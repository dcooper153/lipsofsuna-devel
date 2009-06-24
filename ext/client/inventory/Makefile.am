INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libinventory-cli.la

libinventory_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libinventory_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libinventory_cli_la_SOURCES = \
	ext-inventory.c \
	ext-inventory.h \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-widget.c \
	ext-widget.h
