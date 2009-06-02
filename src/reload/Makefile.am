INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	liblipsreload.la

liblipsreload_la_LDFLAGS = -module -avoid-version
liblipsreload_la_SOURCES = \
	lips-reload.h \
	reload.c \
	reload.h \
	reload-blender.c \
	reload-gimp.c \
	reload-image.c
