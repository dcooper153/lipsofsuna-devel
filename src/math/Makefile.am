INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS)

lib_LTLIBRARIES = \
	liblipsmath.la

liblipsmath_la_SOURCES = \
	lips-math.h \
	math-aabb.h \
	math-convert.h \
	math-frustum.h  \
	math-generic.h \
	math-geometry.h \
	math-halton.h \
	math-matrix.h \
	math-plane.h \
	math-polygon.h \
	math-quadratic.h \
	math-quaternion.h \
	math-rectangle.h \
	math-transform.h \
	math-triangle.h \
	math-types.h \
	math-vector.h \
	math-vertex.h

noinst_PROGRAMS = \
	unittest

unittest_SOURCES = \
	unittest.c

unittest_LDADD = \
	-L. -llipsmath
