INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libnpc-ser.la

libnpc_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libnpc_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libnpc_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-npc.c \
	ext-npc.h \
	ext-script.c
