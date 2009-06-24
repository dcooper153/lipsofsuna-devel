INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libcamera-cli.la

libcamera_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libcamera_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libcamera_cli_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
