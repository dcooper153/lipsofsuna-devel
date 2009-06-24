INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libgenerator-ser.la

libgenerator_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libgenerator_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libgenerator_ser_la_SOURCES = \
	ext-generator.c \
	ext-generator.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
