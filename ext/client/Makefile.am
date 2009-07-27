INCLUDES = -I$(top_srcdir)/src
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
	libwidgets-cli.la

libcamera_cli_la_CFLAGS = $(LICLI_CFLAGS)
libcamera_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libcamera_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libcamera_cli_la_SOURCES = \
	camera/ext-module.c \
	camera/ext-module.h \
	camera/ext-script.c

libchat_cli_la_CFLAGS = $(LICLI_CFLAGS)
libchat_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libchat_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libchat_cli_la_SOURCES = \
	chat/ext-history.c \
	chat/ext-history.h \
	chat/ext-module.c \
	chat/ext-module.h \
	chat/ext-script.c

libeditor_cli_la_CFLAGS = $(LICLI_CFLAGS)
libeditor_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libeditor_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libeditor_cli_la_SOURCES = \
	editor/ext-dialog.c \
	editor/ext-dialog.h \
	editor/ext-editor.c \
	editor/ext-editor.h \
	editor/ext-module.c \
	editor/ext-module.h \
	editor/ext-script.c

libeffect_cli_la_CFLAGS = $(LICLI_CFLAGS)
libeffect_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libeffect_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libeffect_cli_la_SOURCES = \
	effect/ext-module.c \
	effect/ext-module.h \
	effect/ext-script.c

libevents_cli_la_CFLAGS = $(LICLI_CFLAGS)
libevents_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libevents_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libevents_cli_la_SOURCES = \
	events/ext-module.c \
	events/ext-module.h \
	events/ext-script.c

libgenerator_cli_la_CFLAGS = $(LICLI_CFLAGS)
libgenerator_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libgenerator_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libgenerator_cli_la_SOURCES = \
	generator/ext-dialog.c \
	generator/ext-dialog.h \
	generator/ext-generator.c \
	generator/ext-generator.h \
	generator/ext-match.c \
	generator/ext-match.h \
	generator/ext-module.c \
	generator/ext-module.h \
	generator/ext-rule.c \
	generator/ext-rule.h \
	generator/ext-script.c

liboptions_cli_la_CFLAGS = $(LICLI_CFLAGS)
liboptions_cli_la_LDFLAGS = -module -avoid-version -no-undefined
liboptions_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
liboptions_cli_la_SOURCES = \
	options/ext-module.c \
	options/ext-module.h \
	options/ext-options.c \
	options/ext-options.h \
	options/ext-script.c

libpackager_cli_la_CFLAGS = $(LICLI_CFLAGS)
libpackager_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libpackager_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libpackager_cli_la_SOURCES = \
	packager/ext-module.c \
	packager/ext-module.h \
	packager/ext-packager.c \
	packager/ext-packager.h \
	packager/ext-resources.c \
	packager/ext-resources.h \
	packager/ext-script.c

libreload_cli_la_CFLAGS = $(LICLI_CFLAGS)
libreload_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libreload_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libreload_cli_la_SOURCES = \
	reload/ext-module.c \
	reload/ext-module.h \
	reload/ext-reload.c \
	reload/ext-reload.h \
	reload/ext-script.c

libskills_cli_la_CFLAGS = $(LICLI_CFLAGS)
libskills_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libskills_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libskills_cli_la_SOURCES = \
	skills/ext-module.c \
	skills/ext-module.h \
	skills/ext-script.c \
	skills/ext-skills.c \
	skills/ext-skills.h \
	skills/ext-widget.c \
	skills/ext-widget.h

libslots_cli_la_CFLAGS = $(LICLI_CFLAGS)
libslots_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libslots_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libslots_cli_la_SOURCES = \
	slots/ext-module.c \
	slots/ext-module.h \
	slots/ext-slot.c \
	slots/ext-slot.h \
	slots/ext-slots.c \
	slots/ext-slots.h

libwidgets_cli_la_CFLAGS = $(LICLI_CFLAGS)
libwidgets_cli_la_LDFLAGS = -module -avoid-version -no-undefined
libwidgets_cli_la_LIBADD = $(top_builddir)/src/liblipsofsunaclient.la
libwidgets_cli_la_SOURCES = \
	widgets/ext-module.c \
	widgets/ext-module.h \
	widgets/ext-script.c
