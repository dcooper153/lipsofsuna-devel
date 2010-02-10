LIBADD = $(LISRV_LIBS)
AM_CFLAGS = $(LISRV_CFLAGS) -I$(top_srcdir)
AM_LDFLAGS = -module -avoid-version -no-undefined
EXTRA_DIST = skeleton/mkext.sh

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libeditor-ser.la \
	libevents-ser.la \
	libgenerator-ser.la \
	libinventory-ser.la \
	libskills-ser.la \
	libslots-ser.la \
	libvoxel-ser.la

libeditor_ser_la_LIBADD = $(LIBADD)
libeditor_ser_la_SOURCES = \
	editor/ext-module.c \
	editor/ext-module.h \
	editor/ext-script.c

libevents_ser_la_LIBADD = $(LIBADD)
libevents_ser_la_SOURCES = \
	events/ext-module.c \
	events/ext-module.h \
	events/ext-script.c

libgenerator_ser_la_LIBADD = $(LIBADD)
libgenerator_ser_la_SOURCES = \
	generator/ext-generator.c \
	generator/ext-generator.h \
	generator/ext-module.c \
	generator/ext-module.h \
	generator/ext-script.c

libinventory_ser_la_LIBADD = $(LIBADD)
libinventory_ser_la_SOURCES = \
	inventory/ext-inventory.c \
	inventory/ext-inventory.h \
	inventory/ext-module.c \
	inventory/ext-module.h \
	inventory/ext-script.c

libskills_ser_la_LIBADD = $(LIBADD)
libskills_ser_la_SOURCES = \
	skills/ext-module.c \
	skills/ext-module.h \
	skills/ext-skill.c \
	skills/ext-skill.h \
	skills/ext-skills.c \
	skills/ext-skills.h \
	skills/ext-script.c

libslots_ser_la_LIBADD = $(LIBADD)
libslots_ser_la_SOURCES = \
	slots/ext-module.c \
	slots/ext-module.h \
	slots/ext-script.c \
	slots/ext-slots.c \
	slots/ext-slots.h

libvoxel_ser_la_LIBADD = $(LIBADD)
libvoxel_ser_la_SOURCES = \
	voxel/ext-block.c \
	voxel/ext-block.h \
	voxel/ext-listener.c \
	voxel/ext-listener.h \
	voxel/ext-module.c \
	voxel/ext-module.h \
	voxel/ext-script.c

all-local:
	@if test "x$(RELPATH)" = "xyes"; then \
		mkdir -p ../../../build/lib/extensions; \
		for i in $(ext_LTLIBRARIES); do \
			name=`echo $$i | sed s/la$$/so/`; \
			if test -e .libs/$$name; then cp .libs/$$name ../../../build/lib/extensions; fi; \
		done; \
	fi
