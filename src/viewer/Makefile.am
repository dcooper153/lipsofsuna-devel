INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(SDL_CFLAGS) $(OPENGL_CFLAGS) $(BULLET_CFLAGS) $(LUA_CFLAGS)

lib_LTLIBRARIES = \
	liblipsviewer.la

liblipsviewer_la_SOURCES = \
	viewer.c \
	viewer.h

liblipsviewer_la_LIBADD = \
	../reload/liblipsreload.la \
	../engine/liblipsengine.la \
	../script/liblipsscript.la \
	../render/liblipsrender.la \
	../physics/liblipsphysics.la \
	../callback/liblipscallback.la \
	../config/liblipsconfig.la \
	../math/liblipsmath.la \
	../model/liblipsmodel.la \
	../image/liblipsimage.la \
	../video/liblipsvideo.la \
	../archive/liblipsarchive.la \
	../algorithm/liblipsalgorithm.la \
	../string/liblipsstring.la \
	../thread/liblipsthread.la \
	../system/liblipssystem.la \
	$(SDL_LIBS) -lSDL_image $(OPENGL_LIBS) $(BULLET_LIBS) $(LUA_LIBS)
