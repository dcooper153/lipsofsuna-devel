EXTRA_DIST = mkext.sh
INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libskeleton-ser.la

libskeleton_ser_la_LDFLAGS = -module -avoid-version
libskeleton_ser_la_SOURCES = \
	ext-skeleton.c \
	ext-skeleton.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
