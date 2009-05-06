INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libslots-ser.la

libslots_ser_la_LDFLAGS = -module -avoid-version
libslots_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-slots.c \
	ext-slots.h
