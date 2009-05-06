INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libnpc-ser.la

libnpc_ser_la_LDFLAGS = -module -avoid-version
libnpc_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-npc.c \
	ext-npc.h \
	ext-script.c
