SUBDIRS = script .
INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS) $(BULLET_CFLAGS) $(SQLITE_CFLAGS)

lib_LTLIBRARIES = \
	liblipsserver.la

liblipsserver_la_SOURCES = \
	lips-server.h \
	server.c \
	server.h \
	server-callbacks.c \
	server-callbacks.h \
	server-client.c \
	server-client.h \
	server-extension.h \
	server-network.c \
	server-network.h \
	server-object.c \
	server-object.h \
	server-observer.h \
	server-paths.c \
	server-paths.h \
	server-script.h \
	server-types.h

liblipsserver_la_LIBADD = \
	../engine/liblipsengine.la \
	../server/script/liblipsserverscript.la \
	../script/common/liblipscommonscript.la \
	../script/liblipsscript.la \
	../ai/liblipsai.la \
	../physics/liblipsphysics.la \
	../network/liblipsnetwork.la \
	../config/liblipsconfig.la \
	../string/liblipsstring.la \
	../archive/liblipsarchive.la \
	../callback/liblipscallback.la \
	../algorithm/liblipsalgorithm.la \
	../model/liblipsmodel.la \
	../math/liblipsmath.la \
	../system/liblipssystem.la \
	$(LUA_LIBS) $(GRAPPLE_LIBS) $(SQLITE_LIBS)
