dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([Lips of Suna], [0.0.2], [http://lipsofsuna.org], [lipsofsuna])
AC_CONFIG_AUX_DIR([msc])
AC_CONFIG_MACRO_DIR([msc/m4])
AM_INIT_AUTOMAKE([tar-ustar])
AM_CONFIG_HEADER([src/config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
PKG_PROG_PKG_CONFIG
AC_HEADER_STDC

AC_CHECK_HEADERS(dlfcn.h fcntl.h endian.h inttypes.h netdb.h poll.h stdint.h unistd.h windows.h arpa/inet.h netinet/in.h sys/byteorder.h sys/mman.h sys/socket.h sys/stat.h sys/wait.h sys/inotify.h inotifytools/inotify.h)
AC_CHECK_LIB(dl, dlsym)
AC_CHECK_LIB(m, main)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_FUNCS(fork lstat readlink stat)
AC_SEARCH_LIBS(socket, socket)

dnl Arguments.
AC_ARG_ENABLE([debug], AC_HELP_STRING([--disable-debug], [Do not compile with debugging symbols]), [want_debug=no], [want_debug=yes])
AC_ARG_ENABLE([client], AC_HELP_STRING([--disable-client], [Do not compile the client]), [want_client=no], [want_client=yes])
AC_ARG_ENABLE([sound], AC_HELP_STRING([--disable-sound], [Do not compile with sound support]), [want_sound=no], [want_sound=yes])
AC_ARG_ENABLE([server], AC_HELP_STRING([--disable-server], [Do not compile the server]), [want_server=no], [want_server=yes])
AC_ARG_ENABLE([viewer], AC_HELP_STRING([--disable-viewer], [Do not compile the model viewer]), [want_viewer=no], [want_viewer=yes])
AC_ARG_ENABLE([deps], AC_HELP_STRING([--disable-deps], [Do not compile built-in dependencies]), [want_deps=no], [want_deps=yes])
AC_ARG_ENABLE([luatest], AC_HELP_STRING([--disable-luatest], [Do not try to detect whether Lua is available]), [want_luatest=no], [want_luatest=yes])
AC_ARG_ENABLE([relpath], AC_HELP_STRING([--enable-relpath], [Install everything under a single directory]), [want_relative=yes], [want_relative=no])

dnl Compiler flags.
if test "$want_debug" = yes; then
	LIPS_CFLAGS="-g"
else
	LIPS_CFLAGS="-DNDEBUG"
fi

dnl Built-in dependencies.
if test "$want_deps" = yes; then
	MAYBE_DEP="dep"
	BULLET_CFLAGS="-I\$(top_srcdir)/dep/bullet/src"
	BULLET_LIBS="\$(top_builddir)/dep/bullet/src/libbulletdynamics.la \$(top_builddir)/dep/bullet/src/libbulletcollision.la \$(top_builddir)/dep/bullet/src/libbulletmath.la"
	GRAPPLE_CFLAGS="-I\$(top_srcdir)/dep/libgrapple/src"
	GRAPPLE_LIBS="\$(top_builddir)/dep/libgrapple/src/libgrapple.la"
	LUA_CFLAGS="-I\$(top_srcdir)/dep/lua/src"
	LUA_LIBS="\$(top_builddir)/dep/lua/liblua5.1.la"
	SQUISH_CFLAGS="-I\$(top_srcdir)/dep/libsquish"
	SQUISH_LIBS="\$(top_builddir)/dep/libsquish/libsquish.la"
	SQLITE_CFLAGS="-I\$(top_srcdir)/dep/sqlite"
	SQLITE_LIBS="\$(top_builddir)/dep/sqlite/libsqlite3.la"
	ZLIB_CFLAGS="-I\$(top_srcdir)/dep/zlib"
	ZLIB_LIBS="\$(top_builddir)/dep/zlib/libz.la"
	AC_CONFIG_SUBDIRS(dep/zlib)
	AC_CONFIG_SUBDIRS(dep/bullet)
	AC_CONFIG_SUBDIRS(dep/libgrapple)
	AC_CONFIG_SUBDIRS(dep/libsquish)
	AC_CONFIG_SUBDIRS(dep/lua)
	AC_CONFIG_SUBDIRS(dep/sqlite)
else
	MAYBE_DEP=
	PKG_CHECK_MODULES([BULLET], [bullet >= 2.74])
	PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.6.0])
	SQUISH_CFLAGS=""
	SQUISH_LIBS="-lsquish"
	GRAPPLE_CFLAGS=""
	GRAPPLE_LIBS="-lgrapple -lcrypt -lpthread"
	ZLIB_CFLAGS=""
	ZLIB_LIBS="-lz"
	if test "$want_luatest" = yes; then
		PKG_CHECK_MODULES([LUA], [lua5.1 >= 5.1.0], [], [PKG_CHECK_MODULES([LUA], [lua >= 5.1.0])])
	fi
fi

dnl Server support.
if test "$want_server" = yes; then
	MAYBE_SERVER="server"
else
	MAYBE_SERVER=
fi

dnl Client support.
if test "$want_client" = yes; then
	MAYBE_CLIENT="client"
else
	MAYBE_CLIENT=
fi

dnl Viewer support.
if test "$want_viewer" = yes; then
	MAYBE_VIEWER="viewer"
else
	MAYBE_VIEWER=
fi

dnl Graphics support.
if test "$want_client" = yes -o "$want_viewer" = yes; then
	MAYBE_GRAPHICS="video image render font widget binding"
	AM_PATH_SDL
	if test "x$enable_sdltest" = "xyes" ; then
		AC_CHECK_LIB([SDL_image], [IMG_Load], [dummy=1], AC_MSG_ERROR([*** Unable to find SDL_image libary]))
		AC_CHECK_LIB([SDL_ttf], [TTF_Init], [dummy=1], AC_MSG_ERROR([*** Unable to find SDL_ttf library]))
	fi
	OPENGL_CFLAGS="-I/usr/X11R6/include"
	OPENGL_LIBS="-L/usr/X11R6/lib -lGL"
else
	MAYBE_GRAPHICS=
fi

dnl Sound support.
if test "$want_sound" = yes -a "$want_client" = yes; then
	MAYBE_SOUND="sound"
	PKG_CHECK_MODULES([OPENAL], [openal >= 0.0.8])
	PKG_CHECK_MODULES([VORBIS], [vorbis >= 1.2.0 vorbisfile >= 1.2.0])
else
	MAYBE_SOUND=
	OPENAL_CFLAGS="-DLI_DISABLE_SOUND"
fi

dnl Relative paths.
if test "$want_relative" = yes; then
	EXTSDIR="${prefix}/lib/extensions"
	DATADIR="${prefix}/data"
	MODSDIR="${prefix}/mods"
	TOOLDIR="${prefix}/tool"
	SAVEDIR="${prefix}/save"
	bindir="${prefix}"
	LIPS_CFLAGS="$LIPS_CFLAGS -DLI_RELATIVE_PATHS"
	LDFLAGS="-Wl,-rpath -Wl,\\\$\$ORIGIN/lib $LDFLAGS"
else
	EXTSVER="0.1"
	EXTSDIR="${libdir}/${PACKAGE}/extensions/$EXTSVER"
	DATADIR="${datadir}/${PACKAGE}/data"
	MODSDIR="${datadir}/${PACKAGE}/mods"
	TOOLDIR="${datadir}/${PACKAGE}/tool"
	SAVEDIR="${localstatedir}/${PACKAGE}"
	LIPS_CFLAGS="$LIPS_CFLAGS -DLIEXTSDIR=\\\"\"${libdir}/${PACKAGE}/extensions/$EXTSVER\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DLIDATADIR=\\\"\"${datadir}/${PACKAGE}\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DLISAVEDIR=\\\"\"${localstatedir}/${PACKAGE}\"\\\""
fi

dnl Export configuration.
LIPS_CFLAGS="$LIPS_CFLAGS $SQLITE_CFLAGS -Wall -export-dynamic -DLI_ENABLE_ERROR"
AC_SUBST([LIPS_CFLAGS])
AC_SUBST([BULLET_CFLAGS])
AC_SUBST([BULLET_LIBS])
AC_SUBST([GRAPPLE_CFLAGS])
AC_SUBST([GRAPPLE_LIBS])
AC_SUBST([LUA_CFLAGS])
AC_SUBST([LUA_LIBS])
AC_SUBST([OPENAL_CFLAGS])
AC_SUBST([OPENAL_LIBS])
AC_SUBST([OPENGL_CFLAGS])
AC_SUBST([OPENGL_LIBS])
AC_SUBST([SQLITE_CFLAGS])
AC_SUBST([SQLITE_LIBS])
AC_SUBST([SQUISH_CFLAGS])
AC_SUBST([SQUISH_LIBS])
AC_SUBST([VORBIS_CFLAGS])
AC_SUBST([VORBIS_LIBS])
AC_SUBST([ZLIB_CFLAGS])
AC_SUBST([ZLIB_LIBS])
AC_SUBST([MAYBE_CLIENT])
AC_SUBST([MAYBE_DEP])
AC_SUBST([MAYBE_GRAPHICS])
AC_SUBST([MAYBE_SOUND])
AC_SUBST([MAYBE_SERVER])
AC_SUBST([MAYBE_VIEWER])
AC_SUBST([EXTSDIR])
AC_SUBST([DATADIR])
AC_SUBST([MODSDIR])
AC_SUBST([SAVEDIR])
AC_SUBST([TOOLDIR])

AC_OUTPUT([
Makefile
data/Makefile
dep/Makefile
doc/Makefile
ext/Makefile
ext/client/Makefile
ext/client/camera/Makefile
ext/client/chat/Makefile
ext/client/editor/Makefile
ext/client/events/Makefile
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
ext/server/events/Makefile
ext/server/generator/Makefile
ext/server/inventory/Makefile
ext/server/npc/Makefile
ext/server/skeleton/Makefile
ext/server/skills/Makefile
ext/server/slots/Makefile
ext/server/spawner/Makefile
ext/server/voxel/Makefile
src/Makefile
src/ai/Makefile
src/algorithm/Makefile
src/archive/Makefile
src/binding/Makefile
src/callback/Makefile
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
src/reload/Makefile
src/render/Makefile
src/script/Makefile
src/script/common/Makefile
src/server/Makefile
src/server/script/Makefile
src/string/Makefile
src/sound/Makefile
src/system/Makefile
src/thread/Makefile
src/video/Makefile
src/viewer/Makefile
src/widget/Makefile
])
