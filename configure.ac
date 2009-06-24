dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([Lips of Suna], [0.0.2], [http://lipsofsuna.org], [lipsofsuna])
AC_CONFIG_AUX_DIR([msc])
AC_CONFIG_MACRO_DIR([msc/m4])
AC_CANONICAL_SYSTEM
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

dnl Pthread dependency.
ACX_PTHREAD

dnl Arguments.
AC_ARG_ENABLE([easy], AC_HELP_STRING([--enable-easy], [Compile a relative binary using built-in dependencies]), [want_easy=yes], [want_easy=no])
AC_ARG_ENABLE([debug], AC_HELP_STRING([--disable-debug], [Do not compile with debugging symbols]), [want_debug=no], [want_debug=yes])
AC_ARG_ENABLE([client], AC_HELP_STRING([--disable-client], [Do not compile the client]), [want_client=no], [want_client=yes])
AC_ARG_ENABLE([sound], AC_HELP_STRING([--disable-sound], [Do not compile with sound support]), [want_sound=no], [want_sound=yes])
AC_ARG_ENABLE([server], AC_HELP_STRING([--disable-server], [Do not compile the server]), [want_server=no], [want_server=yes])
AC_ARG_ENABLE([viewer], AC_HELP_STRING([--disable-viewer], [Do not compile the model viewer]), [want_viewer=no], [want_viewer=yes])
AC_ARG_ENABLE([luatest], AC_HELP_STRING([--disable-luatest], [Do not try to detect whether Lua is available]), [want_luatest=no], [want_luatest=yes])
AC_ARG_ENABLE([relpath], AC_HELP_STRING([--enable-relpath], [Install everything under a single directory]), [want_relative=yes], [want_relative=no])
AC_ARG_ENABLE([builtin], AC_HELP_STRING([--enable-builtin], [Compile all built-in dependencies]), [want_builtin_all=yes], [])
AC_ARG_ENABLE([builtin-bullet], AC_HELP_STRING([--enable-builtin-bullet], [Compile built-in Bullet]), [want_builtin_bullet=yes], [])
AC_ARG_ENABLE([builtin-glew], AC_HELP_STRING([--enable-builtin-lua], [Compile built-in GLEW]), [want_builtin_glew=yes], [])
AC_ARG_ENABLE([builtin-grapple], AC_HELP_STRING([--enable-builtin-grapple], [Compile built-in Grapple]), [want_builtin_grapple=yes], [])
AC_ARG_ENABLE([builtin-lua], AC_HELP_STRING([--enable-builtin-lua], [Compile built-in Lua]), [want_builtin_lua=yes], [])
AC_ARG_ENABLE([builtin-sqlite], AC_HELP_STRING([--enable-builtin-sqlite], [Compile built-in SQLite]), [want_builtin_sqlite=yes], [])
AC_ARG_ENABLE([builtin-squish], AC_HELP_STRING([--enable-builtin-squish], [Compile built-in Squish]), [want_builtin_squish=yes], [])
AC_ARG_ENABLE([builtin-zlib], AC_HELP_STRING([--enable-builtin-sqlite], [Compile built-in Zlib]), [want_builtin_zlib=yes], [])

if test "$want_easy" = yes; then
	want_relative=yes
	want_builtin_all=yes
	prefix="$PWD/build"
fi
if test "$want_client" = yes -o "$want_viewer" = yes; then
	want_graphics=yes
else
	want_graphics=no
fi
AC_CHECK_HEADERS(dlfcn.h fcntl.h endian.h inttypes.h netdb.h poll.h stdint.h unistd.h windows.h arpa/inet.h netinet/in.h sys/byteorder.h sys/mman.h sys/socket.h sys/stat.h sys/wait.h sys/inotify.h inotifytools/inotify.h GL/glx.h)
AC_CHECK_LIB(dl, dlsym)
AC_CHECK_LIB(m, main)
AC_CHECK_FUNCS(fork lstat readlink stat usleep)
AC_SEARCH_LIBS(socket, socket)
LIPS_CFLAGS="$CFLAGS"
LIPS_LIBS="$LIBS"

dnl Check for SDL.
AM_PATH_SDL([1.2.0], [have_sdl=yes], [have_sdl=no])
if test "$want_graphics" = yes -a "$have_sdl" = no; then
	AC_MSG_ERROR([cannot find SDL]);
fi

dnl Check for OpenGL.
AX_CHECK_GL
OPENGL_CFLAGS="$GL_CFLAGS"
OPENGL_LIBS="$GL_LIBS"
if test "$want_graphics" = yes -a "x$no_gl" = xyes; then
	AC_MSG_ERROR([cannot find OpenGL]);
fi

dnl Compiler flags.
if test "$want_debug" = yes; then
	LIPS_CFLAGS="$LIPS_CFLAGS -g -Wall -DLI_ENABLE_ERROR"
else
	LIPS_CFLAGS="$LIPS_CFLAGS -Wall -DNDEBUG -DLI_ENABLE_ERROR"
fi

dnl All built-in dependencies.
if test "x$want_builtin_all" = xyes; then
	want_builtin_bullet=yes
	want_builtin_glew=yes
	want_builtin_grapple=yes
	want_builtin_lua=yes
	want_builtin_sqlite=yes
	want_builtin_squish=yes
	want_builtin_zlib=yes
fi

dnl Bullet dependency.
if test "x$want_builtin_bullet" = xyes; then
	MAYBE_DEP="$MAYBE_DEP bullet"
	BULLET_CFLAGS="-I\$(top_srcdir)/dep/bullet/src"
	BULLET_LIBS="\$(top_builddir)/dep/bullet/src/libbulletdynamics.la \$(top_builddir)/dep/bullet/src/libbulletcollision.la \$(top_builddir)/dep/bullet/src/libbulletmath.la"
	AC_CONFIG_SUBDIRS(dep/bullet)
else
	PKG_CHECK_MODULES([BULLET], [bullet >= 2.74])
fi

dnl GLEW dependency.
if test "x$want_builtin_glew" = xyes; then
	MAYBE_DEP="$MAYBE_DEP glew"
	GLEW_CFLAGS="-I\$(top_srcdir)/dep/glew/include"
	GLEW_LIBS="\$(top_builddir)/dep/glew/libGLEW.la"
	AC_CONFIG_SUBDIRS(dep/glew)
else
	GLEW_CFLAGS=""
	GLEW_LIBS="-LGLEW"
fi

dnl Grapple dependency.
if test "x$want_builtin_grapple" = xyes; then
	MAYBE_DEP="$MAYBE_DEP libgrapple"
	GRAPPLE_CFLAGS="-I\$(top_srcdir)/dep/libgrapple/src"
	GRAPPLE_LIBS="\$(top_builddir)/dep/libgrapple/src/libgrapple.la"
	AC_CONFIG_SUBDIRS(dep/libgrapple)
else
	GRAPPLE_CFLAGS=""
	GRAPPLE_LIBS="-lgrapple -lcrypt -lpthread"
fi

dnl Lua dependency.
if test "x$want_builtin_lua" = xyes; then
	MAYBE_DEP="$MAYBE_DEP lua"
	LUA_CFLAGS="-I\$(top_srcdir)/dep/lua/src"
	LUA_LIBS="\$(top_builddir)/dep/lua/liblua5.1.la"
	AC_CONFIG_SUBDIRS(dep/lua)
else
	if test "$want_luatest" = yes; then
		PKG_CHECK_MODULES([LUA], [lua5.1 >= 5.1.0], [], [PKG_CHECK_MODULES([LUA], [lua >= 5.1.0])])
	fi
fi

dnl Sqlite dependency.
if test "x$want_builtin_sqlite" = xyes; then
	MAYBE_DEP="$MAYBE_DEP sqlite"
	SQLITE_CFLAGS="-I\$(top_srcdir)/dep/sqlite"
	SQLITE_LIBS="\$(top_builddir)/dep/sqlite/libsqlite3.la"
	AC_CONFIG_SUBDIRS(dep/sqlite)
else
	PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.6.0])
fi

dnl Squish dependency.
if test "x$want_builtin_squish" = xyes; then
	MAYBE_DEP="$MAYBE_DEP libsquish"
	SQUISH_CFLAGS="-I\$(top_srcdir)/dep/libsquish"
	SQUISH_LIBS="\$(top_builddir)/dep/libsquish/libsquish.la"
	AC_CONFIG_SUBDIRS(dep/libsquish)
else
	SQUISH_CFLAGS=""
	SQUISH_LIBS="-lsquish"
fi

dnl Zlib dependency.
if test "x$want_builtin_zlib" = xyes; then
	MAYBE_DEP="$MAYBE_DEP zlib"
	ZLIB_CFLAGS="-I\$(top_srcdir)/dep/zlib"
	ZLIB_LIBS="\$(top_builddir)/dep/zlib/libz.la"
	AC_CONFIG_SUBDIRS(dep/zlib)
else
	ZLIB_CFLAGS=""
	ZLIB_LIBS="-lz"
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

dnl Sound support.
if test "$want_sound" = yes -a "$want_client" = yes; then
	PKG_CHECK_MODULES([OPENAL], [openal >= 0.0.8])
	PKG_CHECK_MODULES([VORBIS], [vorbis >= 1.2.0 vorbisfile >= 1.2.0])
else
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
LISRV_CFLAGS="\
	$LIPS_CFLAGS $PTHREAD_CFLAGS $GRAPPLE_CFLAGS $LUA_CFLAGS $ZLIB_CFLAGS $SQLITE_CFLAGS \
	$SDL_CFLAGS $GL_CFLAGS $GLEW_CFLAGS $OPENAL_CFLAGS $VORBIS_CFLAGS \
	$SQUISH_CFLAGS $BULLET_CFLAGS"
LISRV_LIBS="\
	$LUA_LIBS $GRAPPLE_LIBS $SQLITE_LIBS $BULLET_LIBS $ZLIB_LIBS \
	$PTHREAD_LIBS $PTHREAD_CFLAGS $LIPS_LIBS"
LICLI_CFLAGS="\
	$LIPS_CFLAGS $PTHREAD_CFLAGS $GRAPPLE_CFLAGS $LUA_CFLAGS $ZLIB_CFLAGS $SQLITE_CFLAGS \
	$SDL_CFLAGS $GL_CFLAGS $GLEW_CFLAGS $OPENAL_CFLAGS $VORBIS_CFLAGS \
	$SQUISH_CFLAGS $BULLET_CFLAGS"
LICLI_LIBS="\
	$LUA_LIBS $GRAPPLE_LIBS $SQLITE_LIBS $BULLET_LIBS $ZLIB_LIBS \
	$SDL_LIBS -lSDL_ttf -lSDL_image \
	$GLEW_LIBS $GL_LIBS $OPENAL_LIBS $VORBIS_LIBS $SQUISH_LIBS \
	$PTHREAD_LIBS $PTHREAD_CFLAGS $LIPS_LIBS"
AC_SUBST([LISRV_CFLAGS])
AC_SUBST([LISRV_LIBS])
AC_SUBST([LICLI_CFLAGS])
AC_SUBST([LICLI_LIBS])
AC_SUBST([LIPS_CFLAGS])
AC_SUBST([LIPS_LIBS])
AC_SUBST([BULLET_CFLAGS])
AC_SUBST([BULLET_LIBS])
AC_SUBST([GLEW_CFLAGS])
AC_SUBST([GLEW_LIBS])
AC_SUBST([GRAPPLE_CFLAGS])
AC_SUBST([GRAPPLE_LIBS])
AC_SUBST([LUA_CFLAGS])
AC_SUBST([LUA_LIBS])
AC_SUBST([OPENAL_CFLAGS])
AC_SUBST([OPENAL_LIBS])
AC_SUBST([OPENGL_CFLAGS])
AC_SUBST([OPENGL_LIBS])
AC_SUBST([PTHREAD_CFLAGS])
AC_SUBST([PTHREAD_LIBS])
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
])
