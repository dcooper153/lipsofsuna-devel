INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libinventory-ser.la

libinventory_ser_la_LDFLAGS = -module -avoid-version
libinventory_ser_la_SOURCES = \
	ext-inventory.c \
	ext-inventory.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
