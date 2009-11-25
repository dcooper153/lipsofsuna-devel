AC_INIT([Lips of Suna], [0.0.3], [http://lipsofsuna.org], [lipsofsuna])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([silent-rules subdir-objects tar-ustar])
AM_CONFIG_HEADER([lipsofsuna/config.h])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

dnl Arguments.
AC_ARG_ENABLE([easy], AC_HELP_STRING([--enable-easy], [Compile a relative binary and install it to ./build]), [want_easy=yes], [want_easy=no])
AC_ARG_ENABLE([debug], AC_HELP_STRING([--disable-debug], [Do not compile with debugging symbols]), [want_debug=no], [want_debug=yes])
AC_ARG_ENABLE([optimization], AC_HELP_STRING([--disable-optimization], [Do not compile with optimization]), [want_optimization=no], [want_optimization=yes])
AC_ARG_ENABLE([client], AC_HELP_STRING([--disable-client], [Do not compile the client]), [want_client=no], [want_client=yes])
AC_ARG_ENABLE([generator], AC_HELP_STRING([--disable-generator], [Do not compile the generator]), [want_generator=no], [want_generator=yes])
AC_ARG_ENABLE([import], AC_HELP_STRING([--disable-import], [Do not compile the data file importer]), [want_import=no], [want_import=yes])
AC_ARG_ENABLE([server], AC_HELP_STRING([--disable-server], [Do not compile the server]), [want_server=no], [want_server=yes])
AC_ARG_ENABLE([sound], AC_HELP_STRING([--disable-sound], [Do not compile with sound support]), [want_sound=no], [want_sound=yes])
AC_ARG_ENABLE([viewer], AC_HELP_STRING([--disable-viewer], [Do not compile the model viewer]), [want_viewer=no], [want_viewer=yes])
AC_ARG_ENABLE([relpath], AC_HELP_STRING([--enable-relpath], [Install everything under a single directory]), [want_relative=yes], [want_relative=no])
AC_ARG_ENABLE([malloc-debug], AC_HELP_STRING([--enable-malloc-debug], [Compile with logging memory management functions]), [want_mallocdebug=yes], [want_mallocdebug=no])
AC_ARG_WITH([sdl-config], AC_HELP_STRING([--with-sdl-config=PTH], [Path to sdl-config (optional)]), [SDL_CONFIG="$withval"], [])
AC_ARG_WITH([dep-prefix],
	AC_HELP_STRING([--with-dep-prefix=PFX], [Prefix where to search for dependencies (optional)]), [
	CFLAGS="$CFLAGS -I$withval/include"
	CPPFLAGS="$CPPFLAGS -I$withval/include"
	CXXFLAGS="$CXXFLAGS -I$withval/include"
	LDFLAGS="$LDFLAGS -L$withval/lib"
	if test "x$PKG_CONFIG_PATH" = x; then
		PKG_CONFIG_PATH="$withval/lib/pkgconfig"
	else
		PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$withval/lib/pkgconfig"
	fi
	dnl No way to avoid this?
	export PKG_CONFIG_PATH], [])

dnl Compiler flags.
if test "x$want_debug" = xyes; then
	CFLAGS="$CFLAGS -g"
else
	CFLAGS="$CFLAGS -DNDEBUG"
fi
if test "x$want_optimization" = xyes; then
	CFLAGS="$CFLAGS -O3"
fi
if test "x$want_mallocdebug" = xyes; then
	CFLAGS="$CFLAGS -DLI_MALLOC_DEBUG"
fi

dnl Tools, headers, libraries.
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
PKG_PROG_PKG_CONFIG
AC_HEADER_STDC
AC_PATH_X
ACX_PTHREAD
AC_CHECK_HEADERS([dlfcn.h fcntl.h inttypes.h netdb.h poll.h stdint.h unistd.h])
AC_CHECK_HEADERS([time.h sys/time.h])
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h sys/socket.h])
AC_CHECK_HEADERS([endian.h sys/byteorder.h])
AC_CHECK_HEADERS([sys/mman.h sys/stat.h sys/wait.h])
AC_CHECK_HEADERS([sys/inotify.h inotifytools/inotify.h])
AC_CHECK_HEADERS([GL/glx.h])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([rpcndr.h], [], [],
[#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
])
AC_CHECK_LIB(dl, dlsym)
AC_CHECK_LIB(m, main)
AC_CHECK_FUNCS(fork lstat readlink stat usleep)
AC_SEARCH_LIBS(socket, socket)

LIPS_CFLAGS="$CFLAGS -Wall -DLI_ENABLE_ERROR"
LIPS_LIBS="$LIBS"

AC_ARG_VAR([GL_CFLAGS], [C compiler flags for OpenGL])
AC_ARG_VAR([GL_LIBS], [linker flags for OpenGL [-lGL]])
AC_ARG_VAR([GLEW_CFLAGS], [C compiler flags for GLEW])
AC_ARG_VAR([GLEW_LIBS], [linker flags for GLEW [-lGLEW]])
AC_ARG_VAR([GRAPPLE_CFLAGS], [C compiler flags for grapple])
AC_ARG_VAR([GRAPPLE_LIBS], [linker flags for grapple [-lgrapple]])
AC_ARG_VAR([SDL_CONFIG], [path to sdl-config])
AC_ARG_VAR([SDL_CFLAGS], [C compiler flags for SDL])
AC_ARG_VAR([SDL_LIBS], [linker flags for SDL])
AC_ARG_VAR([SQUISH_CFLAGS], [C compiler flags for squish])
AC_ARG_VAR([SQUISH_LIBS], [linker flags for squish [-lsquish]])
AC_ARG_VAR([ZLIB_CFLAGS], [C compiler flags for zlib])
AC_ARG_VAR([ZLIB_LIBS], [linker flags for zlib [-lz]])

dnl Compilation options.
if test "x$want_easy" = xyes; then
	want_relative=yes
	prefix="$PWD/build"
fi

dnl Dependencies.
if test "x$want_client" = xyes || test "x$want_server" = xyes; then
	PKG_CHECK_MODULES([LUA], [lua5.1 >= 5.1.0], [found=yes], [found=no])
	if test "x$found" = "xno"; then
		PKG_CHECK_MODULES([LUA], [lua >= 5.1.0])
	fi
	if test "x$found" = xno; then
		AC_CHECK_LIB([lua5.1], [lua_newtable], [founda=yes], [founda=no])
		AC_CHECK_LIB([lua], [lua_newtable], [foundb=yes], [foundb=no])
		AC_CHECK_HEADER([lua.h], [foundc=yes], [foundc=no])
		AC_CHECK_HEADER([lauxlib.h], [foundd=yes], [foundd=no])
		if ( test "x$founda" = xno && test "x$foundb" = xno ) || test "x$foundc" = xno || test "x$foundd" = xno; then
			AC_MSG_ERROR([

Cannot find Lua. You may set the environment variables
LUA_CFLAGS and LUA_LIBS to bypass this test.])
		fi
		if test "x$founda" = xyes; then
			LUA_CFLAGS=""
			LUA_LIBS="-llua5.1"
		else
			LUA_CFLAGS=""
			LUA_LIBS="-llua"
		fi
	fi
fi
if test "x$want_client" = xyes || test "x$want_import" = xyes || test "x$want_viewer" = xyes; then
	PKG_CHECK_MODULES([LIBPNG], [libpng12 >= 1.2.0], [found=yes], [found=no])
	if test "x$found" = xno; then
		AC_CHECK_LIB([png], [spng_create_read_struct], [founda=yes], [founda=no])
		AC_CHECK_HEADER([png.h], [foundb=yes], [foundb=no])
		if test "x$founda" = xno || test "x$foundb" = xno; then
			AC_MSG_ERROR([

Cannot find libpng. You may set the environment variables
LIBPNG_CFLAGS and LIBPNG_LIBS to bypass this test.])
		else
			LIBPNG_CFLAGS=""
			LIBPNG_LIBS="-lpng"
		fi
	fi
fi
if test "x$want_sound" = xyes && test "x$want_client" = xyes; then
	PKG_CHECK_MODULES([OPENAL], [openal >= 0.0.8])
	PKG_CHECK_MODULES([VORBIS], [vorbis >= 1.2.0 vorbisfile >= 1.2.0])
else
	OPENAL_CFLAGS="-DLI_DISABLE_SOUND"
fi
if test xyes = xyes; then
	PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.6.0], [found=yes], [found=no])
	if test "x$found" = xno; then
		AC_CHECK_LIB([sqlite3], [sqlite3_open], [founda=yes], [founda=no])
		AC_CHECK_HEADER([sqlite3.h], [foundb=yes], [foundb=no])
		if test "x$founda" = xno || test "x$foundb" = xno; then
			AC_MSG_ERROR([

Cannot find SQLite. You may set the environment variables
SQLITE_CFLAGS and SQLITE_LIBS to bypass this test.])
		else
			SQLITE_CFLAGS=""
			SQLITE_LIBS="-lsqlite3"
		fi
	fi
fi
if test "x$want_client" = xyes || test "x$want_server" = xyes; then
	PKG_CHECK_MODULES([BULLET], [bullet >= 2.74], [found=yes], [found=no])
	if test "x$found" = xno; then
		AC_CHECK_LIB([bulletdynamics], [main], [founda=yes], [founda=no])
		AC_LANG_PUSH([C++])
		AC_CHECK_HEADER([btBulletCollisionCommon.h], [foundb=yes], [foundb=no])
		AC_CHECK_HEADER([btBulletDynamicsCommon.h], [foundc=yes], [foundc=no])
		AC_LANG_POP()
		if test "x$founda" = xno || test "x$foundb" = xno || test "x$foundc" = xno; then
			AC_MSG_ERROR([

Cannot find Bullet. You may set the environment variables
BULLET_CFLAGS and BULLET_LIBS to bypass this test.])
		else
			BULLET_CFLAGS=""
			BULLET_LIBS="-lbulletdynamics -lbulletcollision -lbulletmath"
		fi
	fi
fi
if test "x$want_client" = yes || test "x$want_viewer" = xyes; then
	if test "x$SDL_CFLAGS" = "x" && test "x$SDL_LIBS" = "x"; then
		if test "x$SDL_CONFIG" = "x"; then
			AM_PATH_SDL([1.2.0], [found=yes], [found=no])
			if test "x$found" = xno; then
				AC_MSG_ERROR([

Cannot find SDL. You may set the environment variables
SDL_CONFIG, SDL_CFLAGS, and SDL_LIBS to bypass this test.])
			fi
		else
			SDL_CFLAGS="`$SDL_CONFIG --cflags`"
			SDL_LIBS="`$SDL_CONFIG --libs`"
		fi
	fi
fi
if test "x$want_client" = yes || test "x$want_viewer" = xyes; then
	if test "x$GL_CFLAGS" = "x" && test "x$GL_LIBS" = "x"; then
		AX_CHECK_GL
		if test "x$no_gl" = "xyes"; then
			AC_MSG_ERROR([

Cannot find OpenGL. You may set the environment variables
GL_CFLAGS and GL_LIBS to bypass this test.])
		fi
	fi
fi
if test "x$want_client" = yes || test "x$want_viewer" = xyes; then
	if test "x$GLEW_CFLAGS" = "x" && test "x$GLEW_LIBS" = "x"; then
		AC_CHECK_LIB([GLEW], [glewInit], [founda=yes], [founda=no])
		AC_CHECK_HEADER([GL/glew.h], [foundb=yes], [foundb=no])
		if test "x$founda" = xno || test "x$foundb" = xno; then
			AC_MSG_ERROR([

Cannot find GLEW. You may set the environment variables
GLEW_CFLAGS and GLEW_LIBS to bypass this test.])
		fi
		GLEW_CFLAGS=""
		GLEW_LIBS="-lGLEW"
	fi
fi
if test "x$want_client" = xyes || test "x$want_server" = xyes; then
	if test "x$GRAPPLE_CFLAGS" = "x" && test "x$GRAPPLE_LIBS" = "x"; then
		AC_CHECK_LIB([grapple], [grapple_server_default_get], [found=yes], [found=no])
		AC_CHECK_HEADER([grapple/grapple_client.h], [foundb=yes], [foundb=no])
		AC_CHECK_HEADER([grapple/grapple_server.h], [foundc=yes], [foundc=no])
		if test "x$founda" = xno || test "x$foundb" = xno || test "x$foundc" = xno; then
			AC_MSG_ERROR([

Cannot find grapple. You may set the environment variables
GRAPPLE_CFLAGS and GRAPPLE_LIBS to bypass this test.])
		else
			GRAPPLE_CFLAGS=""
			GRAPPLE_LIBS="-lgrapple"
		fi
	fi
fi
if test "$want_client" = xyes || test "x$want_viewer" = xyes || test "x$want_import" = xyes; then
	if test "x$SQUISH_CFLAGS" = "x" && test "x$SQUISH_LIBS" = "x"; then
		AC_CHECK_LIB([squish], [main], [founda=yes], [founda=no])
		AC_LANG_PUSH([C++])
		AC_CHECK_HEADER([squish.h], [foundb=yes], [foundb=no])
		AC_LANG_POP()
		if test "x$founda" = xno || test "x$foundb" = xno; then
			AC_MSG_ERROR([

Cannot find squish. You may set the environment variables
SQUISH_CFLAGS and SQUISH_LIBS to bypass this test.])
		else
			SQUISH_CFLAGS=""
			SQUISH_LIBS="-lsquish"
		fi
	fi
fi
if test xyes = xyes; then
	if test "x$ZLIB_CFLAGS" = "x" && test "x$ZLIB_LIBS" = "x"; then
		AC_CHECK_LIB([z], [deflate], [found=yes], [found=no])
		AC_CHECK_HEADER([zlib.h], [foundb=yes], [foundb=no])
		if test "x$founda" = xno || test "x$foundb" = xno; then
			AC_MSG_ERROR([

Cannot find zlib. You may set the environment variables
ZLIB_CFLAGS and ZLIB_LIBS to bypass this test.])
		else
			ZLIB_CFLAGS=""
			ZLIB_LIBS="-lz"
		fi
	fi
fi

dnl Relative paths.
if test "$want_relative" = yes; then
	EXTSDIR="${prefix}/lib/extensions"
	DATADIR="${prefix}/data"
	MODSDIR="${prefix}/mods"
	PROGDIR="${prefix}"
	TOOLDIR="${prefix}/tool"
	SAVEDIR="${prefix}/save"
	LIPS_CFLAGS="$LIPS_CFLAGS -DLI_RELATIVE_PATHS"
	LDFLAGS="-Wl,-rpath -Wl,\\\$\$ORIGIN/../lib $LDFLAGS"
else
	EXTSVER="0.1"
	EXTSDIR="${libdir}/${PACKAGE}/extensions/$EXTSVER"
	DATADIR="${datadir}/${PACKAGE}/data"
	MODSDIR="${datadir}/${PACKAGE}/mods"
	PROGDIR="${bindir}"
	TOOLDIR="${datadir}/${PACKAGE}/tool"
	SAVEDIR="${localstatedir}/${PACKAGE}"
	LIPS_CFLAGS="$LIPS_CFLAGS -DLIEXTSDIR=\\\"\"${libdir}/${PACKAGE}/extensions/$EXTSVER\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DLIPROGDIR=\\\"\"${bindir}\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DLIDATADIR=\\\"\"${datadir}/${PACKAGE}\"\\\""
	LIPS_CFLAGS="$LIPS_CFLAGS -DLISAVEDIR=\\\"\"${localstatedir}/${PACKAGE}\"\\\""
fi

dnl Targets.
AM_CONDITIONAL([MAYBE_CLIENT], [test "$want_client" = yes])
AM_CONDITIONAL([MAYBE_GENERATOR], [test "$want_generator" = yes])
AM_CONDITIONAL([MAYBE_IMPORT], [test "$want_import" = yes])
AM_CONDITIONAL([MAYBE_SERVER], [test "$want_server" = yes])
AM_CONDITIONAL([MAYBE_VIEWER], [test "$want_viewer" = yes])

dnl Export.
LISRV_CFLAGS="\
	$LIPS_CFLAGS $PTHREAD_CFLAGS $GRAPPLE_CFLAGS $LUA_CFLAGS $ZLIB_CFLAGS $SQLITE_CFLAGS \
	$SDL_CFLAGS $GL_CFLAGS $GLEW_CFLAGS $OPENAL_CFLAGS $VORBIS_CFLAGS \
	$SQUISH_CFLAGS $BULLET_CFLAGS"
LISRV_LIBS="\
	$LUA_LIBS $GRAPPLE_LIBS $SQLITE_LIBS $BULLET_LIBS $ZLIB_LIBS \
	$PTHREAD_LIBS $LIPS_LIBS"
LICLI_CFLAGS="\
	$GRAPPLE_CFLAGS $LUA_CFLAGS $SQLITE_CFLAGS $BULLET_CFLAGS \
	$SDL_CFLAGS $GLEW_CFLAGS $GL_CFLAGS $OPENAL_CFLAGS $VORBIS_CFLAGS \
	$SQUISH_CFLAGS $ZLIB_CFLAGS $LIBPNG_CFLAGS $PTHREAD_CFLAGS $LIPS_CFLAGS"
LICLI_LIBS="\
	$GRAPPLE_LIBS $LUA_LIBS $SQLITE_LIBS $BULLET_LIBS $GLEW_LIBS \
	$GL_LIBS $OPENAL_LIBS $VORBIS_LIBS $SQUISH_LIBS $ZLIB_LIBS \
	$LIBPNG_LIBS $PTHREAD_LIBS $LIPS_LIBS"
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
AC_SUBST([EXTSDIR])
AC_SUBST([DATADIR])
AC_SUBST([MODSDIR])
AC_SUBST([PROGDIR])
AC_SUBST([SAVEDIR])
AC_SUBST([TOOLDIR])

AC_OUTPUT([
Makefile
doc/Makefile
lipsofsuna/Makefile
lipsofsuna/client/extensions/Makefile
lipsofsuna/server/extensions/Makefile
])
