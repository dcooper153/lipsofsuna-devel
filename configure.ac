dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([Lips of Suna], [0.0.1], [http://lipsofsuna.org], [lipsofsuna])
AC_CONFIG_AUX_DIR([msc])
AC_CONFIG_MACRO_DIR([msc/m4])
AM_INIT_AUTOMAKE(AC_PACKAGE_TARNAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(src/config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
PKG_PROG_PKG_CONFIG
AC_HEADER_STDC

AC_CHECK_HEADERS(arpa/inet.h fcntl.h endian.h inttypes.h netdb.h netinet/in.h stdint.h sys/byteorder.h sys/socket.h)
AC_CHECK_HEADERS(sys/inotify.h inotifytools/inotify.h)
AC_CHECK_LIB(dl, dlsym)
AC_CHECK_LIB(m, main)
AC_CHECK_LIB(z, deflate)
AC_CHECK_LIB(pthread, pthread_create)
AC_SEARCH_LIBS(socket, socket)
PKG_CHECK_MODULES([BULLET], [bullet >= 2.71])

dnl FIXME
GRAPPLE_CFLAGS=""
GRAPPLE_LIBS="-lgrapple -lcrypt -lpthread"
AC_SUBST([GRAPPLE_CFLAGS])
AC_SUBST([GRAPPLE_LIBS])

dnl Arguments.
AC_ARG_ENABLE([debug], AC_HELP_STRING([--disable-debug], [Do not compile with debugging symbols]), [want_debug=no], [want_debug=yes])
AC_ARG_ENABLE([client], AC_HELP_STRING([--disable-client], [Do not compile the client]), [want_client=no], [want_client=yes])
AC_ARG_ENABLE([sound], AC_HELP_STRING([--disable-sound], [Do not compile with sound support]), [want_sound=no], [want_sound=yes])
AC_ARG_ENABLE([server], AC_HELP_STRING([--disable-server], [Do not compile the server]), [want_server=no], [want_server=yes])
AC_ARG_ENABLE([luatest], AC_HELP_STRING([--disable-luatest], [Do not try to detect whether Lua is available]), [want_luatest=no], [want_luatest=yes])
AC_ARG_ENABLE([relpath], AC_HELP_STRING([--enable-relpath], [Install everything under a single directory]), [want_relative=yes], [want_relative=no])

dnl Compilation flags.
if test "$want_debug" = yes; then
	LIPS_CFLAGS="-g"
else
	LIPS_CFLAGS="-DNDEBUG"
fi
LIPS_CFLAGS="$LIPS_CFLAGS -Wall -export-dynamic -DLI_ENABLE_ERROR"

dnl Conditional client compilation.
if test "$want_client" = yes; then
	MAYBE_CLIENT="client"
else
	MAYBE_CLIENT=
fi

dnl Conditional graphics compilation if client or editor wanted.
if test "$want_client" = yes -o "$want_editor" = yes; then
	MAYBE_GRAPHICS="video image render font widget binding"

	dnl Check for SDL.
	AM_PATH_SDL
	if test "x$enable_sdltest" = "xyes" ; then
		AC_CHECK_LIB([SDL_image], [IMG_Load], [dummy=1], AC_MSG_ERROR([*** Unable to find SDL_image libary]))
		AC_CHECK_LIB([SDL_ttf], [TTF_Init], [dummy=1], AC_MSG_ERROR([*** Unable to find SDL_ttf library]))
	fi

	dnl Check for OpenGL.
	OPENGL_CFLAGS="-I/usr/X11R6/include"
	OPENGL_LIBS="-L/usr/X11R6/lib -lGL"
	AC_SUBST([OPENGL_CFLAGS])
	AC_SUBST([OPENGL_LIBS])
else
	MAYBE_GRAPHICS=
fi

dnl Conditional sound compilation.
if test "$want_sound" = yes -a \( "$want_client" = yes -o "$want_editor" = yes \); then
	MAYBE_SOUND="sound"

	dnl Check for OpenAL.
	PKG_CHECK_MODULES([OPENAL], [openal >= 0.0.8])
	AC_SUBST([OPENAL_LIBS])
	AC_SUBST([OPENAL_CFLAGS])

	dnl Check for Vorbis.
	PKG_CHECK_MODULES([VORBIS], [vorbis >= 1.2.0 vorbisfile >= 1.2.0])
	AC_SUBST([VORBIS_LIBS])
	AC_SUBST([VORBIS_CFLAGS])
else
	MAYBE_SOUND=

	OPENAL_CFLAGS="-DLI_DISABLE_SOUND"
	AC_SUBST([OPENAL_CFLAGS])
fi

dnl Conditional server compilation.
if test "$want_server" = yes; then
	MAYBE_SERVER="server"
else
	MAYBE_SERVER=
fi

dnl Conditional Lua check.
if test "$want_server" = yes; then
	if test "$want_luatest" = yes; then
		PKG_CHECK_MODULES([LUA], [lua5.1 >= 5.1.0], [],
		[PKG_CHECK_MODULES([LUA], [lua >= 5.1.0])])
	fi
	AC_SUBST([LUA_CFLAGS])
	AC_SUBST([LUA_LIBS])
fi

dnl Relative paths.
if test "$want_relative" = yes; then
	EXTSDIR="${prefix}/lib/extensions"
	DATADIR="${prefix}/data"
	MODSDIR="${prefix}/mods"
	SAVEDIR="${prefix}/save"
	bindir="${prefix}"
	LIPS_CFLAGS="$LIPS_CFLAGS -DLI_RELATIVE_PATHS"
	LDFLAGS="-Wl,-rpath -Wl,\\\$\$ORIGIN/lib $LDFLAGS"
else
	EXTSVER="0.1"
	EXTSDIR="${libdir}/${PACKAGE}/extensions/$EXTSVER"
	DATADIR="${datadir}/${PACKAGE}/data"
	MODSDIR="${datadir}/${PACKAGE}/mods"
	SAVEDIR="${localstatedir}/${PACKAGE}"
	LIPS_CFLAGS="$LIPS_CFLAGS -DEXTSDIR=\\\"\"${libdir}/${PACKAGE}/extensions/$EXTSVER\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DDATADIR=\\\"\"${datadir}/${PACKAGE}\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DSAVEDIR=\\\"\"${localstatedir}/${PACKAGE}\"\\\""
fi

dnl Export configuration.
AC_SUBST([LIPS_CFLAGS])
AC_SUBST([MAYBE_CLIENT])
AC_SUBST([MAYBE_GRAPHICS])
AC_SUBST([MAYBE_SOUND])
AC_SUBST([MAYBE_SERVER])
AC_SUBST([EXTSDIR])
AC_SUBST([DATADIR])
AC_SUBST([MODSDIR])
AC_SUBST([SAVEDIR])

AC_OUTPUT([
Makefile
data/Makefile
data/editor/Makefile
doc/Makefile
ext/Makefile
ext/client/Makefile
ext/client/camera/Makefile
ext/client/chat/Makefile
ext/client/editor/Makefile
ext/client/generator/Makefile
ext/client/inventory/Makefile
ext/client/options/Makefile
ext/client/packager/Makefile
ext/client/reload/Makefile
ext/client/skills/Makefile
ext/client/slots/Makefile
ext/client/widgets/Makefile
ext/server/Makefile
ext/server/creature/Makefile
ext/server/editor/Makefile
ext/server/generator/Makefile
ext/server/inventory/Makefile
ext/server/npc/Makefile
ext/server/skills/Makefile
ext/server/slots/Makefile
ext/server/spawner/Makefile
src/Makefile
src/ai/Makefile
src/algorithm/Makefile
src/archive/Makefile
src/binding/Makefile
src/callback/Makefile
src/class/Makefile
src/client/Makefile
src/client/script/Makefile
src/config/Makefile
src/engine/Makefile
src/font/Makefile
src/image/Makefile
src/main/Makefile
src/math/Makefile
src/model/Makefile
src/network/Makefile
src/physics/Makefile
src/render/Makefile
src/script/Makefile
src/script/common/Makefile
src/sector/Makefile
src/server/Makefile
src/server/script/Makefile
src/string/Makefile
src/sound/Makefile
src/system/Makefile
src/thread/Makefile
src/video/Makefile
src/widget/Makefile
])
