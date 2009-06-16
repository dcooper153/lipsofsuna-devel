INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libvoxel-ser.la

libvoxel_ser_la_LDFLAGS = -module -avoid-version
libvoxel_ser_la_SOURCES = \
	ext-listener.c \
	ext-listener.h \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-sector.c \
	ext-sector.h
