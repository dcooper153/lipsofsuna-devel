ACLOCAL_AMFLAGS = -I msc/m4
INCLUDES = -I$(srcdir)/include
AM_CFLAGS = -g -Wall -DGLEW_BUILD $(GL_CFLAGS)

lib_LTLIBRARIES = \
	libGLEW.la

libGLEW_la_LDFLAGS = -no-undefined
libGLEW_la_LIBADD = $(GL_LIBS)
libGLEW_la_SOURCES = \
	src/glew.c \
	src/glewinfo.c \
	include/GL/glew.h \
	include/GL/glxew.h \
	include/GL/wglew.h

#	src/visualinfo.c
