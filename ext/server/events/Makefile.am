INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libevents-ser.la

libevents_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libevents_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libevents_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
