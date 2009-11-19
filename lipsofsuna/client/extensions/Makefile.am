INCLUDES = -I$(top_srcdir)
AM_CFLAGS = $(LICLI_CFLAGS)

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libcamera-cli.la \
	libchat-cli.la \
	libeditor-cli.la \
	libeffect-cli.la \
	libevents-cli.la \
	libgenerator-cli.la \
	liboptions-cli.la \
	libpackager-cli.la \
	libreload-cli.la \
	libskills-cli.la \
	libslots-cli.la \
	libsound-cli.la \
	libvoxel-cli.la \
	libwidgets-cli.la

libcamera_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libcamera_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libcamera_cli_la_SOURCES = \
	camera/ext-module.c \
	camera/ext-module.h \
	camera/ext-script.c

libchat_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libchat_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libchat_cli_la_SOURCES = \
	chat/ext-history.c \
	chat/ext-history.h \
	chat/ext-module.c \
	chat/ext-module.h \
	chat/ext-script.c

libeditor_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libeditor_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libeditor_cli_la_SOURCES = \
	editor/ext-dialog.c \
	editor/ext-dialog.h \
	editor/ext-editor.c \
	editor/ext-editor.h \
	editor/ext-module.c \
	editor/ext-module.h \
	editor/ext-script.c

libeffect_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libeffect_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libeffect_cli_la_SOURCES = \
	effect/ext-module.c \
	effect/ext-module.h \
	effect/ext-script.c

libevents_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libevents_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libevents_cli_la_SOURCES = \
	events/ext-module.c \
	events/ext-module.h \
	events/ext-script.c

libgenerator_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libgenerator_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libgenerator_cli_la_SOURCES = \
	generator/ext-brushes.c \
	generator/ext-brushes.h \
	generator/ext-dialog.c \
	generator/ext-dialog.h \
	generator/ext-materials.c \
	generator/ext-materials.h \
	generator/ext-module.c \
	generator/ext-module.h \
	generator/ext-preview.c \
	generator/ext-preview.h \
	generator/ext-script.c

liboptions_cli_la_LDFLAGS = -module -avoid-version -no-undefined
liboptions_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
liboptions_cli_la_SOURCES = \
	options/ext-module.c \
	options/ext-module.h \
	options/ext-options.c \
	options/ext-options.h \
	options/ext-script.c

libpackager_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libpackager_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libpackager_cli_la_SOURCES = \
	packager/ext-module.c \
	packager/ext-module.h \
	packager/ext-packager.c \
	packager/ext-packager.h \
	packager/ext-resources.c \
	packager/ext-resources.h \
	packager/ext-script.c

libreload_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libreload_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libreload_cli_la_SOURCES = \
	reload/ext-module.c \
	reload/ext-module.h \
	reload/ext-reload.c \
	reload/ext-reload.h \
	reload/ext-script.c

libskills_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libskills_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libskills_cli_la_SOURCES = \
	skills/ext-module.c \
	skills/ext-module.h \
	skills/ext-script.c \
	skills/ext-skills.c \
	skills/ext-skills.h \
	skills/ext-widget.c \
	skills/ext-widget.h

libslots_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libslots_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libslots_cli_la_SOURCES = \
	slots/ext-module.c \
	slots/ext-module.h \
	slots/ext-slot.c \
	slots/ext-slot.h \
	slots/ext-slots.c \
	slots/ext-slots.h

libsound_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libsound_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libsound_cli_la_SOURCES = \
	sound/ext-module.c \
	sound/ext-module.h \
	sound/ext-script.c

libvoxel_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libvoxel_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libvoxel_cli_la_SOURCES = \
	voxel/ext-block.c \
	voxel/ext-block.h \
	voxel/ext-module.c \
	voxel/ext-module.h \
	voxel/ext-script.c

libwidgets_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libwidgets_cli_la_LIBADD = $(top_builddir)/lipsofsuna/liblipsofsunaclient.la
libwidgets_cli_la_SOURCES = \
	widgets/ext-module.c \
	widgets/ext-module.h \
	widgets/ext-script.c
