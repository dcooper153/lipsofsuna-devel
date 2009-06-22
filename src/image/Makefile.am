INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(SDL_CFLAGS) $(OPENGL_CFLAGS) $(SQUISH_CFLAGS)
AM_CXXFLAGS = $(LIPS_CFLAGS) $(SDL_CFLAGS) $(OPENGL_CFLAGS) $(SQUISH_CFLAGS)

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

liblipsimage_la_LIBADD = $(SQUISH_LIBS)
