INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libvoxel-ser.la

libvoxel_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libvoxel_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libvoxel_ser_la_SOURCES = \
	ext-listener.c \
	ext-listener.h \
	ext-module.c \
	ext-module.h \
	ext-script.c \
	ext-sector.c \
	ext-sector.h
