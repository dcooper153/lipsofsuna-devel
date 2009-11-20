ACLOCAL_AMFLAGS = -I m4
INCLUDES = -I$(srcdir)
AM_CFLAGS = -g -Wall
EXTRA_DIST = singlecolourlookup.inl

lib_LTLIBRARIES = \
	libsquish.la

libsquish_la_LDFLAGS = -no-undefined
libsquish_la_SOURCES = \
	alpha.cpp \
	alpha.h \
	clusterfit.cpp \
	clusterfit.h \
	colourblock.cpp \
	colourblock.h \
	colourfit.cpp \
	colourfit.h \
	colourset.cpp \
	colourset.h \
	config.h \
	maths.cpp \
	maths.h \
	rangefit.cpp \
	rangefit.h \
	simd_float.h \
	simd.h \
	simd_sse.h \
	simd_ve.h \
	singlecolourfit.cpp \
	singlecolourfit.h \
	squish.cpp \
	squish.h
