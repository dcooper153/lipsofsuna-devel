INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libinventory-ser.la

libinventory_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libinventory_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libinventory_ser_la_SOURCES = \
	ext-inventory.c \
	ext-inventory.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
