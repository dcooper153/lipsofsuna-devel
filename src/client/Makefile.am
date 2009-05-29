SUBDIRS = script .
INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(SDL_CFLAGS) $(OPENGL_CFLAGS) $(OPENAL_CFLAGS) $(VORBIS_CFLAGS) $(GRAPPLE_CFLAGS)

lib_LTLIBRARIES = \
	liblipsclient.la

liblipsclient_la_SOURCES = \
	client.c \
	client.h \
	client-callbacks.c \
	client-callbacks.h \
	client-command.c \
	client-event.h \
	client-extension.h \
	client-module.c \
	client-module.h \
	client-network.c \
	client-network.h \
	client-object.c \
	client-object.h \
	client-paths.c \
	client-paths.h \
	client-script.h \
	client-speech.c \
	client-speech.h \
	client-types.h \
	client-window.c \
	client-window.h \
	lips-client.h \
	widget-login.c \
	widget-login.h

liblipsclient_la_LIBADD = \
	../server/liblipsserver.la \
	../client/script/liblipsclientscript.la \
	../script/common/liblipscommonscript.la \
	../script/liblipsscript.la \
	../engine/liblipsengine.la \
	../ai/liblipsai.la \
	../render/liblipsrender.la \
	../physics/liblipsphysics.la \
	../network/liblipsnetwork.la \
	../config/liblipsconfig.la \
	../math/liblipsmath.la \
	../model/liblipsmodel.la \
	../widget/liblipswidget.la \
	../font/liblipsfont.la \
	../image/liblipsimage.la \
	../binding/liblipsbinding.la \
	../sound/liblipssound.la \
	../video/liblipsvideo.la \
	../archive/liblipsarchive.la \
	../callback/liblipscallback.la \
	../algorithm/liblipsalgorithm.la \
	../string/liblipsstring.la \
	../thread/liblipsthread.la \
	../system/liblipssystem.la \
	$(SDL_LIBS) -lSDL_ttf -lSDL_image \
	$(LUA_LIBS) $(OPENGL_LIBS) $(OPENAL_LIBS) $(VORBIS_LIBS) $(GRAPPLE_LIBS)
