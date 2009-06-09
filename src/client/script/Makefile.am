INCLUDES = -I$(srcdir)/../..
AM_CFLAGS = \
        $(LIPS_CFLAGS) \
        $(LUA_CFLAGS) \
        $(SDL_CFLAGS) \
        $(OPENGL_CFLAGS) \
        $(OPENAL_CFLAGS) \
        $(VORBIS_CFLAGS) \
        $(GRAPPLE_CFLAGS)

lib_LTLIBRARIES = \
	liblipsclientscript.la

liblipsclientscript_la_SOURCES = \
	lips-client-script.h \
	script-action.c \
	script-binding.c \
	script-extension.c \
	script-group.c \
	script-module.c \
	script-object.c \
	script-player.c \
	script-widget.c \
	script-window.c
