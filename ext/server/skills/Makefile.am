INCLUDES = -I$(srcdir)/../../../src
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libskills-ser.la

libskills_ser_la_LDFLAGS = -module -avoid-version
libskills_ser_la_SOURCES = \
	ext-module.c \
	ext-module.h \
	ext-skill.c \
	ext-skill.h \
	ext-skills.c \
	ext-skills.h \
	ext-script.c
