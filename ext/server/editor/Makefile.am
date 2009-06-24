INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libeditor-ser.la

libeditor_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libeditor_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libeditor_ser_la_SOURCES = \
	ext-editor.c \
	ext-editor.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
