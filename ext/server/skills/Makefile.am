INCLUDES = -I$(top_srcdir)/src
AM_CFLAGS = $(LISRV_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = libskills-ser.la

libskills_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libskills_ser_la_LIBADD = $(top_builddir)/src/liblipsofsunaserver.la
libskills_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-skill.c \
	ext-skill.h \
	ext-skills.c \
	ext-skills.h \
	ext-script.c
