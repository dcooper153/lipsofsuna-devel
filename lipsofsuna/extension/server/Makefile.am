LIBADD = $(LISRV_LIBS)
AM_CFLAGS = $(LISRV_CFLAGS) -I$(top_srcdir)
AM_LDFLAGS = -module -avoid-version -no-undefined

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libevents-ser.la \
	libinventory-ser.la \
	libvoxel-ser.la

libevents_ser_la_LIBADD = $(LIBADD)
libevents_ser_la_SOURCES = \
	events/ext-module.c \
	events/ext-module.h \
	events/ext-script.c

libinventory_ser_la_LIBADD = $(LIBADD)
libinventory_ser_la_SOURCES = \
	inventory/ext-inventory.c \
	inventory/ext-inventory.h \
	inventory/ext-module.c \
	inventory/ext-module.h \
	inventory/ext-script.c

libvoxel_ser_la_LIBADD = $(LIBADD)
libvoxel_ser_la_SOURCES = \
	voxel/ext-block.c \
	voxel/ext-block.h \
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
