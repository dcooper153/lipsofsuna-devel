INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libcreature-ser.la

libcreature_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libcreature_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libcreature_ser_la_SOURCES = \
	ext-creature.c \
	ext-creature.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
