INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libslots-ser.la

libslots_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libslots_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libslots_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-slots.c \
	ext-slots.h
