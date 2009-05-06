INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libgenerator-ser.la

libgenerator_ser_la_LDFLAGS = -module -avoid-version
libgenerator_ser_la_SOURCES = \
	ext-generator.c \
	ext-generator.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
