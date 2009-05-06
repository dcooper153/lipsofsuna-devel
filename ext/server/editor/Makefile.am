INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libeditor-ser.la

libeditor_ser_la_LDFLAGS = -module -avoid-version
libeditor_ser_la_SOURCES = \
	ext-editor.c \
	ext-editor.h \
	ext-module.c \
	ext-module.h \
	ext-script.c
