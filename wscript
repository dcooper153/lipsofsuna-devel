import os
import string
import Options
import Utils

APPNAME='lipsofsuna'
VERSION='0.0.3'

srcdir = '.'
blddir = 'build'

CORE_DIRS = 'ai algorithm archive binding callback client engine extension font generator image main math model network particle paths physics reload render script server sound string system thread video voxel widget'
EXTS_DIRS = 'binding camera chat creature effect generator network npc packager region reload skeleton slots sound spawner speech tiles tiles-physics tiles-render widgets'
CORE_EXCL = 'lipsofsuna/math/unittest.c lipsofsuna/server/server-main.c lipsofsuna/main/main.c'

def set_options(ctx):
	ctx.tool_options('compiler_cc')
	ctx.tool_options('compiler_cxx')
	ctx.add_option('--relpath', action='store', default=True, help='relative data and library paths [default: true]')
	ctx.add_option('--bindir', action='store', default=None, help='override executable directory [default: PREFIX/bin]')
	ctx.add_option('--libdir', action='store', default=None, help='override library directory [default: PREFIX/lib]')
	ctx.add_option('--datadir', action='store', default=None, help='override data directory [default: PREFIX/share]')
	ctx.add_option('--savedir', action='store', default=None, help='override save directory [default: PREFIX/var]')
	ctx.add_option('--adddeps', action='store', default=None, help='extra path to dependencies')
	ctx.add_option('--sound', action='store', default=True, help='compile with sound support [default: true]')

def configure(ctx):

	# Options
	if Options.options.relpath != "false" and (Options.options.bindir or Options.options.libdir or Options.options.datadir or Options.options.savedir):
		print("\nThe directory overrides are not used by a relative path build")
		print("To enable a traditional build, configure with --relpath=false\n")
		exit(1)
	ctx.env.RELPATH = Options.options.relpath != "false"
	ctx.env.SOUND = Options.options.sound != "false"

	# Directories
	ctx.env.CPPPATH_CORE = ['.']
	ctx.env.CPPFLAGS_CORE = ['-g', '-Wall', '-O0', '-DHAVE_CONFIG_H']
	ctx.env.LIBPATH_CORE = []
	ctx.env.LINKFLAGS_CORE = ['-g', '-export-dynamic']
	if Options.options.adddeps:
		binpaths = []
		pkgpaths = []
		for dep in string.split(Options.options.adddeps, ':'):
			ctx.env.CPPPATH_CORE.append(os.path.join(dep, 'include'))
			ctx.env.LIBPATH_CORE.append(os.path.join(dep, 'lib'))
			binpaths.append(os.path.join(dep, 'bin'))
			pkgpaths.append(os.path.join(dep, 'lib', 'pkgconfig'))
		if 'PATH' in os.environ and os.environ['PATH'] != '':
			binpaths.append(os.environ['PATH'])
		os.environ['PATH'] = string.join(binpaths, ':')
		if 'PKG_CONFIG_PATH' in os.environ and os.environ['PKG_CONFIG_PATH'] != '':
			pkgpaths.append(os.environ['PKG_CONFIG_PATH'])
		os.environ['PKG_CONFIG_PATH'] = string.join(pkgpaths, ':')
	ctx.env.CPPPATH_EXTENSION = ctx.env.CPPPATH_CORE
	ctx.env.CPPFLAGS_EXTENSION = list(ctx.env.CPPFLAGS_CORE)
	ctx.env.CPPFLAGS_EXTENSION.append('-DDLL_EXPORT')
	ctx.env.LIBPATH_EXTENSION = ctx.env.LIBPATH_CORE
	ctx.env.LINKFLAGS_EXTENSION = ['-g']
	ctx.env.CPPPATH_TEST = []
	ctx.env.LIBPATH_TEST = []

	# Tools
	ctx.check_tool('compiler_cc')
	ctx.check_tool('compiler_cxx')

	# Dependencies
	ctx.check(header_name='arpa/inet.h', define_name='HAVE_ARPA_INET_H')
	ctx.check(header_name='dlfcn.h', define_name='HAVE_DLFCN_H')
	ctx.check(header_name='endian.h', define_name='HAVE_ENDIAN_H')
	ctx.check(header_name='fcntl.h', define_name='HAVE_FCNTL_H')
	ctx.check(header_name='inotifytools/inotify.h', define_name='HAVE_INOTIFYTOOLS_INOTIFY_H')
	ctx.check(header_name='inttypes.h', define_name='HAVE_INTTYPES_H')
	ctx.check(header_name='netdb.h', define_name='HAVE_NETDB_H')
	ctx.check(header_name='netinet/in.h', define_name='HAVE_NETINET_IN_H')
	ctx.check(header_name='poll.h', define_name='HAVE_POLL_H')
	ctx.check(header_name='stdint.h', define_name='HAVE_STDINT_H')
	ctx.check(header_name='sys/byteorder.h', define_name='HAVE_SYS_BYTEORDER_H')
	ctx.check(header_name='sys/inotify.h', define_name='HAVE_SYS_INOTIFY_H')
	ctx.check(header_name='sys/mman.h', define_name='HAVE_SYS_MMAN_H')
	ctx.check(header_name='sys/socket.h', define_name='HAVE_SYS_SOCKET_H')
	ctx.check(header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
	ctx.check(header_name='sys/time.h', define_name='HAVE_SYS_TIME_H')
	ctx.check(header_name='sys/wait.h', define_name='HAVE_SYS_WAIT_H')
	ctx.check(header_name='time.h', define_name='HAVE_TIME_H')
	ctx.check(header_name='unistd.h', define_name='HAVE_UNISTD_H')
	ctx.check(header_name='windows.h', define_name='HAVE_WINDOWS_H')
	ctx.check(header_name='GL/glx.h', define_name='HAVE_GL_GLX_H')
	ctx.check_cc(msg='Checking for function fork', header_name='unistd.h', function_name='fork', define_name='HAVE_FORK')
	ctx.check_cc(msg='Checking for function usleep', header_name='unistd.h', function_name='usleep', define_name='HAVE_USLEEP')
	ctx.check_cc(lib='dl', uselib_store='CORE')
	ctx.check_cc(lib='m', uselib_store='CORE')
	ctx.check_cc(lib='pthread', uselib_store='THREAD')
	if ctx.is_defined('HAVE_WINDOWS_H'):
		ctx.env.EXEEXT = '.exe'
	else:
		ctx.env.EXEEXT = ''

	# zlib
	ctx.check_cc(lib='z', mandatory=True, uselib_store='ZLIB')
	ctx.check_cc(header_name='zlib.h', mandatory=True, uselib_store='ZLIB')

	# SQLite
	if not ctx.check_cfg(package='sqlite', atleast_version='3.6.0', args='--cflags --libs'):
		ctx.check_cc(header_name='sqlite3.h', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')
		ctx.check_cc(lib='sqlite3', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')

	# Bullet
	if not ctx.check_cfg(package='bullet', atleast_version='2.74', args='--cflags --libs'):
		ctx.check_cxx(header_name='btBulletCollisionCommon.h', mandatory=True, uselib='CORE TEST', uselib_store='BULLET')
		if not ctx.check_cxx(lib='linearmath', uselib='CORE TEST', uselib_store='BULLET'):
			ctx.check_cxx(lib='LinearMath', mandatory=True, uselib='CORE TEST', uselib_store='BULLET')
		if not ctx.check_cxx(lib='bulletcollision', uselib='CORE TEST', uselib_store='BULLET'):
			ctx.check_cxx(lib='BulletCollision', mandatory=True, uselib='CORE TEST', uselib_store='BULLET')
		if not ctx.check_cxx(lib='bulletdynamics', uselib='CORE TEST', uselib_store='BULLET'):
			ctx.check_cxx(lib='BulletDynamics', mandatory=True, uselib='CORE TEST', uselib_store='BULLET')

	# Grapple
	if not ctx.check_cfg(package='grapple', atleast_version='0.9', args='--cflags --libs'):
		ctx.check_cc(header_name='grapple/grapple_client.h', mandatory=True, uselib='CORE TEST', uselib_store='GRAPPLE')
		#ctx.check_cc(header_name='grapple/grapple_server.h', mandatory=True, uselib='CORE TEST', uselib_store='GRAPPLE')
		ctx.check_cc(lib='grapple', mandatory=True, uselib='CORE TEST', uselib_store='GRAPPLE')

	# Lua
	if not ctx.check_cfg(package='lua5.1', atleast_version='5.1', args='--cflags --libs', uselib_store='LUA'):
		if not ctx.check_cfg(package='lua', atleast_version='5.1', args='--cflags --libs'):
			ctx.check_cc(header_name='lua.h', mandatory=True, uselib='CORE TEST', uselib_store='LUA')
			ctx.check_cc(header_name='lauxlib.h', mandatory=True, uselib='CORE TEST', uselib_store='LUA')
			if not ctx.check_cc(lib='lua5.1', uselib='CORE TEST', uselib_store='LUA'):
				ctx.check_cc(lib='lua', mandatory=True, uselib='CORE TEST', uselib_store='LUA')

	# SDL
	if not ctx.check_cfg(package='sdl', atleast_version='1.2.0', args='--cflags --libs'):
		ctx.check_cxx(header_name='SDL.h', mandatory=True, uselib='CORE TEST', uselib_store='SDL')
		ctx.check_cxx(lib='SDL', mandatory=True, uselib='CORE TEST', uselib_store='SDL')

	# SDL_ttf
	ctx.check_cc(lib='SDL_ttf', mandatory=True, uselib='CORE TEST SDL', uselib_store='SDL_TTF')
	ctx.check_cc(msg = "Checking for header SDL_ttf.h", mandatory=True, uselib='CORE TEST SDL', fragment="#include <SDL_ttf.h>\nint main(int argc, char** argv) { return 0; }\n")

	# GL
	ctx.check_cc(header_name='GL/gl.h', mandatory=True, uselib='CORE TEST', uselib_store='GL')
	if not ctx.check_cc(lib='GL', uselib='CORE TEST', uselib_store='GL'):
		ctx.check_cc(lib='opengl32', mandatory=True, uselib='CORE TEST', uselib_store='GL')

	# GLEW
	ctx.check_cc(header_name='GL/glew.h', mandatory=True, uselib='CORE TEST', uselib_store='GLEW')
	ctx.check_cc(lib='GLEW', mandatory=True, uselib='CORE TEST GL', uselib_store='GLEW')

	# PNG
	if not ctx.check_cfg(package='libpng12', atleast_version='1.2.0', args='--cflags --libs', uselib_store="PNG") and \
	   not ctx.check_cfg(package='libpng14', atleast_version='1.2.0', args='--cflags --libs', uselib_store="PNG") and \
	   not ctx.check_cfg(package='libpng', atleast_version='1.2.0', args='--cflags --libs', uselib_store="PNG"):
		ctx.check_cxx(header_name='png.h', mandatory=True, uselib='CORE TEST', uselib_store='PNG')
		if not ctx.check_cxx(lib='png12', mandatory=True, uselib='CORE TEST', uselib_store='PNG') and \
		   not ctx.check_cxx(lib='png15', mandatory=True, uselib='CORE TEST', uselib_store='PNG'):
			ctx.check_cxx(lib='png', mandatory=True, uselib='CORE TEST', uselib_store='PNG')

	# squish
	ctx.check_cxx(header_name='squish.h', mandatory=True, uselib='CORE TEST', uselib_store='SQUISH')
	ctx.check_cxx(lib='squish', mandatory=True, uselib='CORE TEST', uselib_store='SQUISH')

	if ctx.env.SOUND:
		# AL
		if not ctx.check_cfg(package='openal', atleast_version='0.0.8', args='--cflags --libs', uselib_store="AL"):
			ctx.check_cc(header_name='AL/al.h', mandatory=True, uselib='CORE TEST', uselib_store='AL')
			ctx.check_cc(lib='openal', mandatory=True, uselib='CORE TEST', uselib_store='AL')
		# OGG
		if not ctx.check_cfg(package='ogg', atleast_version='1.1.0', args='--cflags --libs'):
			ctx.check_cc(header_name='ogg.h', mandatory=True, uselib='CORE TEST', uselib_store='OGG')
			ctx.check_cc(lib='ogg', mandatory=True, uselib='CORE TEST', uselib_store='OGG')
		# VORBIS
		if not ctx.check_cfg(package='vorbis', atleast_version='1.2.0', args='--cflags --libs') or \
		   not ctx.check_cfg(package='vorbisfile', atleast_version='1.2.0', args='--cflags --libs', uselib_store='VORBIS'):
			ctx.check_cc(header_name='vorbis.h', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
			ctx.check_cc(header_name='vorbisfile.h', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
			ctx.check_cc(lib='vorbis', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
			ctx.check_cc(lib='vorbisfile', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
		# FLAC
		if not ctx.check_cfg(package='flac', atleast_version='1.2.0', args='--cflags --libs'):
			ctx.check_cc(header_name='stream_decoder.h', mandatory=True, uselib='CORE TEST', uselib_store='FLAC')
			ctx.check_cc(lib='FLAC', mandatory=True, uselib='CORE TEST', uselib_store='FLAC')

	# Defines
	ctx.define('LI_ENABLE_ERROR', 1)
	if not ctx.env.SOUND:
		ctx.define('LI_DISABLE_SOUND', 1)
	if ctx.env.RELPATH:
		ctx.define('LI_RELATIVE_PATHS', 1)
		ctx.env.RPATH_CORE = ['$ORIGIN/lib']
	else:
		bindir = Options.options.bindir
		if not bindir:
			bindir = os.path.join(ctx.env.PREFIX, 'bin')
		libdir = Options.options.libdir
		if not libdir:
			libdir = os.path.join(ctx.env.PREFIX, 'lib')
		datadir = Options.options.datadir
		if not datadir:
			datadir = os.path.join(ctx.env.PREFIX, 'share')
		savedir = Options.options.savedir
		if not savedir:
			savedir = os.path.join(ctx.env.PREFIX, 'var')
		ctx.env.BINDIR = bindir
		ctx.env.EXTSDIR = os.path.join(libdir, APPNAME, 'extensions', '0.1')
		ctx.env.DATADIR = os.path.join(datadir, APPNAME, 'data')
		ctx.env.MODSDIR = os.path.join(datadir, APPNAME, 'mods')
		ctx.env.PROGDIR = bindir
		ctx.env.TOOLDIR = os.path.join(datadir, APPNAME, 'tool')
		ctx.env.SAVEDIR = os.path.join(savedir, APPNAME)
		ctx.define('LIEXTSDIR', ctx.env.EXTSDIR)
		ctx.define('LIDATADIR', ctx.env.DATADIR)
		ctx.define('LIMODSDIR', ctx.env.MODSDIR)
		ctx.define('LIPROGDIR', ctx.env.PROGDIR)
		ctx.define('LITOOLDIR', ctx.env.TOOLDIR)
		ctx.define('LISAVEDIR', ctx.env.SAVEDIR)
	ctx.write_config_header('config.h')

	# Messages
	if ctx.env.RELPATH:
		ctx.define('LI_RELATIVE_PATHS', 1)
		print("\nConfigured with:")
		print("\trelative paths\n")
	else:
		print("\nConfigured with:")
		print("\tbindir: " + bindir)
		print("\tlibdir: " + libdir)
		print("\tdatadir: " + datadir)
		print("\tsavedir: " + savedir + "\n")
	print("Build command: waf")
	print("Install command: waf install\n")

def build(ctx):
	ctx.add_group("build")
	ctx.add_group("install")
	ctx.set_group("build")
	if ctx.env.RELPATH:
		ctx.env.PREFIX = os.path.join(ctx.path.abspath(), 'install')
		ctx.env.BINDIR = ctx.env.PREFIX
		ctx.env.EXTSDIR = os.path.join(ctx.env.PREFIX, 'lib', 'extensions')
		ctx.env.DATADIR = os.path.join(ctx.env.PREFIX, 'data')
		ctx.env.MODSDIR = os.path.join(ctx.env.PREFIX, 'mods')
		ctx.env.PROGDIR = os.path.join(ctx.env.PREFIX, 'bin')
		ctx.env.TOOLDIR = os.path.join(ctx.env.PREFIX, 'tool')
		ctx.env.SAVEDIR = os.path.join(ctx.env.PREFIX, 'save')
	objs = ''
	libs = 'CORE LUA SQLITE BULLET GRAPPLE SDL SDL_TTF ZLIB GLEW GL PNG SQUISH THREAD AL VORBIS OGG FLAC'

	# Core objects.
	for dir in CORE_DIRS.split(' '):
		path = os.path.join('lipsofsuna', dir, '*.c')
		srcs = ctx.path.ant_glob(path, excl = CORE_EXCL.split(' '))
		if srcs:
			objs += dir + '_objs '
			ctx.new_task_gen(
				features = 'cc',
				source = srcs,
				target = dir + '_objs',
				uselib = libs)
		path = os.path.join('lipsofsuna', dir, '*.cpp')
		srcs = ctx.path.ant_glob(path, excl = CORE_EXCL.split(' '))
		if srcs:
			objs += dir + '_cxx_objs '
			ctx.new_task_gen(
				features = 'cxx',
				source = srcs,
				target = dir + '_cxx_objs',
				uselib = libs)

	# Extension objects.
	for dir in EXTS_DIRS.split(' '):
		path = os.path.join('lipsofsuna', 'extension', dir, '*.c')
		objs += dir + '_ext_objs '
		ctx.new_task_gen(
			features = 'cc',
			source = ctx.path.ant_glob(path),
			target = dir + '_ext_objs',
			uselib = 'EXTENSION LUA SQLITE GRAPPLE SDL SDL_TTF ZLIB GLEW GL PNG THREAD AL VORBIS OGG FLAC')

	# Target executable.
	ctx.new_task_gen(
		features = 'cc cxx cprogram',
		target = 'lipsofsuna-bin',
		install_path = None,
		add_objects = objs,
		uselib = libs)

	# Installation.
	ctx.set_group("install")
	instpath = os.path.join(ctx.env.BINDIR, 'lipsofsuna' + ctx.env.EXEEXT)
	ctx.install_as(instpath, 'lipsofsuna-bin' + ctx.env.EXEEXT, chmod = 0777)
	ctx.install_files(ctx.env.TOOLDIR, ['lipsofsuna/reload/blender-export.py'])
	for src in ctx.path.ant_glob('data/**/*.*').split(' '):
		dst = os.path.join(ctx.env.DATADIR, os.path.dirname(src).replace('data/', ''))
		ctx.install_files(dst, [src])

def html(ctx):
	Utils.exec_command('docs/makedoc.sh ' + VERSION)
