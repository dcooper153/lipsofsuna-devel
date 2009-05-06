INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libspawner-ser.la

libspawner_ser_la_LDFLAGS = -module -avoid-version
libspawner_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-spawner.c \
	ext-spawner.h \
	ext-script.c
