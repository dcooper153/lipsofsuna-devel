INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libevents-ser.la

libevents_ser_la_LDFLAGS = -module -avoid-version
libevents_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c
