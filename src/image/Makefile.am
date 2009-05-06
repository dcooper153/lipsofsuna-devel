INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../../dep/build/include
AM_CFLAGS = $(LIPS_CFLAGS) $(SDL_CFLAGS) $(OPENGL_CFLAGS)

lib_LTLIBRARIES = \
	liblipsimage.la

liblipsimage_la_SOURCES = \
	image.c \
	image.h \
	image-compress.cpp \
	image-compress.h \
	image-dds.h \
	image-texture.c \
	image-texture.h \
	lips-image.h

liblipsimage_la_LIBADD = \
	-L$(srcdir)/../../dep/build/lib -lsquish
