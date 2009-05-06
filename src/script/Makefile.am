SUBDIRS = . common
INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS)

lib_LTLIBRARIES = \
	liblipsscript.la

liblipsscript_la_SOURCES = \
	lips-script.h \
	script.c \
	script.h \
	script-class.c \
	script-class.h \
	script-data.c \
	script-data.h \
	script-private.h \
	script-types.h
