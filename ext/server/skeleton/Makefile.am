EXTRA_DIST = mkext.sh
INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libskeleton-ser.la

libskeleton_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libskeleton_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libskeleton_ser_la_SOURCES = \
	ext-skeleton.c \
	ext-skeleton.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
