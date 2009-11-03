INCLUDES = -I$(top_srcdir)
AM_CFLAGS = $(LISRV_CFLAGS)
EXTRA_DIST = skeleton/mkext.sh

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libcreature-ser.la \
	libeditor-ser.la \
	libevents-ser.la \
	libgenerator-ser.la \
	libinventory-ser.la \
	libnpc-ser.la \
	libskeleton-ser.la \
	libskills-ser.la \
	libslots-ser.la \
	libspawner-ser.la \
	libvoxel-ser.la

libcreature_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libcreature_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libcreature_ser_la_SOURCES = \
	creature/ext-creature.c \
	creature/ext-creature.h \
	creature/ext-module.c \
	creature/ext-module.h \
	creature/ext-script.c

libeditor_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libeditor_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libeditor_ser_la_SOURCES = \
	editor/ext-editor.c \
	editor/ext-editor.h \
	editor/ext-module.c \
	editor/ext-module.h \
	editor/ext-script.c

libevents_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libevents_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libevents_ser_la_SOURCES = \
	events/ext-module.c \
	events/ext-module.h \
	events/ext-script.c

libgenerator_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libgenerator_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libgenerator_ser_la_SOURCES = \
	generator/ext-generator.c \
	generator/ext-generator.h \
	generator/ext-module.c \
	generator/ext-module.h \
	generator/ext-script.c

libinventory_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libinventory_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libinventory_ser_la_SOURCES = \
	inventory/ext-inventory.c \
	inventory/ext-inventory.h \
	inventory/ext-module.c \
	inventory/ext-module.h \
	inventory/ext-script.c

libnpc_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libnpc_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libnpc_ser_la_SOURCES = \
	npc/ext-module.c \
	npc/ext-module.h \
	npc/ext-npc.c \
	npc/ext-npc.h \
	npc/ext-script.c

libskeleton_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libskeleton_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libskeleton_ser_la_SOURCES = \
	skeleton/ext-skeleton.c \
	skeleton/ext-skeleton.h \
	skeleton/ext-module.c \
	skeleton/ext-module.h \
	skeleton/ext-script.c

libskills_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libskills_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libskills_ser_la_SOURCES = \
	skills/ext-module.c \
	skills/ext-module.h \
	skills/ext-skill.c \
	skills/ext-skill.h \
	skills/ext-skills.c \
	skills/ext-skills.h \
	skills/ext-script.c

libslots_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libslots_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libslots_ser_la_SOURCES = \
	slots/ext-module.c \
	slots/ext-module.h \
	slots/ext-script.c \
	slots/ext-slots.c \
	slots/ext-slots.h

libspawner_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libspawner_ser_la_LIBADD =$(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libspawner_ser_la_SOURCES = \
	spawner/ext-module.c \
	spawner/ext-module.h \
	spawner/ext-spawner.c \
	spawner/ext-spawner.h \
	spawner/ext-script.c

libvoxel_ser_la_LDFLAGS = -module -avoid-version -no-undefined
libvoxel_ser_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaserver.la
libvoxel_ser_la_SOURCES = \
	voxel/ext-block.c \
	voxel/ext-block.h \
	voxel/ext-listener.c \
	voxel/ext-listener.h \
	voxel/ext-module.c \
	voxel/ext-module.h \
	voxel/ext-script.c
