INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libspawner-ser.la

libspawner_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libspawner_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libspawner_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-spawner.c \
	ext-spawner.h \
	ext-script.c
