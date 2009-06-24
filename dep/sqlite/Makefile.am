ACLOCAL_AMFLAGS = -I msc/m4
INCLUDES = -I$(srcdir)
AM_CFLAGS = -g -Wall

lib_LTLIBRARIES = \
	libsqlite3.la

libsqlite3_la_LDFLAGS = -no-undefined
libsqlite3_la_SOURCES = \
	sqlite3.c \
	sqlite3.h \
	sqlite3ext.h
