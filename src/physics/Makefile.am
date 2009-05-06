INCLUDES = -I$(srcdir)/..
AM_CXXFLAGS = $(LIPS_CFLAGS) $(BULLET_CFLAGS)

lib_LTLIBRARIES = \
	liblipsphysics.la

liblipsphysics_la_SOURCES = \
	lips-physics.h \
	physics.cpp \
	physics.h \
	physics-object.cpp \
	physics-object.h \
	physics-private.cpp \
	physics-private.h \
	physics-shape.cpp \
	physics-shape.h \
	physics-types.h

liblipsphysics_la_LIBADD = \
	$(BULLET_LIBS)
