INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libcreature-ser.la

libcreature_ser_la_LDFLAGS = -module -avoid-version
libcreature_ser_la_SOURCES = \
	ext-creature.c \
	ext-creature.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
