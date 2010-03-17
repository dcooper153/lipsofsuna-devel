import os
import string
import Options
import Utils

APPNAME='lipsofsuna'
VERSION='0.0.3'

srcdir = '.'
blddir = 'build'

def set_options(ctx):
	ctx.tool_options('compiler_cc')
	ctx.tool_options('compiler_cxx')
	ctx.add_option('--relpath', action='store', default=True, help='relative data and library paths [default: true]')
	ctx.add_option('--bindir', action='store', default=None, help='override executable directory [default: PREFIX/bin]')
	ctx.add_option('--libdir', action='store', default=None, help='override library directory [default: PREFIX/lib]')
	ctx.add_option('--datadir', action='store', default=None, help='override data directory [default: PREFIX/share]')
	ctx.add_option('--savedir', action='store', default=None, help='override save directory [default: PREFIX/var]')
	ctx.add_option('--adddeps', action='store', default=None, help='extra path to dependencies')
	ctx.add_option('--client', action='store', default=True, help='compile client [default: true]')
	ctx.add_option('--generator', action='store', default=True, help='compile generator [default: true]')
	ctx.add_option('--importer', action='store', default=True, help='compile importer [default: true]')
	ctx.add_option('--server', action='store', default=True, help='compile server [default: true]')
	ctx.add_option('--sound', action='store', default=True, help='compile with sound support [default: true]')
	ctx.add_option('--viewer', action='store', default=True, help='compile viewer [default: true]')

def configure(ctx):
	# Options
	if Options.options.relpath != "false" and (Options.options.bindir or Options.options.libdir or Options.options.datadir or Options.options.savedir):
		print("\nThe directory overrides are not used by a relative path build")
		print("To enable a traditional build, configure with --relpath=false\n")
		exit(1)
	ctx.env.CLIENT = Options.options.client != "false"
	ctx.env.GENERATOR = Options.options.generator != "false"
	ctx.env.IMPORT = Options.options.importer != "false"
	ctx.env.RELPATH = Options.options.relpath != "false"
	ctx.env.SERVER = Options.options.server != "false"
	ctx.env.SOUND = Options.options.sound != "false"
	ctx.env.VIEWER = Options.options.viewer != "false"
	# Directories
	ctx.env.CPPPATH_CORE = ['.']
	ctx.env.CPPFLAGS_CORE = ['-g', '-Wall', '-O2', '-DHAVE_CONFIG_H']
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
	if True:
		# zlib
		ctx.check_cc(lib='z', mandatory=True, uselib_store='ZLIB')
		ctx.check_cc(header_name='zlib.h', mandatory=True, uselib_store='ZLIB')
		# SQLite
		if not ctx.check_cfg(package='sqlite', atleast_version='3.6.0', args='--cflags --libs'):
			ctx.check_cc(header_name='sqlite3.h', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')
			ctx.check_cc(lib='sqlite3', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')
	if ctx.env.CLIENT or ctx.env.SERVER or ctx.env.GENERATOR:
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
	if ctx.env.CLIENT or ctx.env.VIEWER or ctx.env.IMPORT:
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
	if ctx.env.CLIENT and ctx.env.SOUND:
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
		ctx.env.RPATH_CORE = ['$ORIGIN/../lib']
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
		ctx.env.RPATH_CORE = ['$ORIGIN/../lib']
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
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/ai/*.c'),
		target = 'ai_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/algorithm/*.c'),
		target = 'alg_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/archive/*.c'),
		target = 'arc_objs',
		uselib = 'CORE ZLIB')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/binding/*.c'),
		target = 'bnd_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/callback/*.c'),
		target = 'cal_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/client/*.c', excl=['lipsofsuna/client/client-main.c']),
		target = 'cli_objs',
		uselib = 'CORE LUA GRAPPLE SDL SDL_TTF GL GLEW')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/engine/*.c'),
		target = 'eng_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/font/*.c'),
		target = 'fnt_objs',
		uselib = 'CORE SDL SDL_TTF')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/generator/*.c', excl=['lipsofsuna/generator/generator-main.c']),
		target = 'gen_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/image/*.c'),
		target = 'img_objs',
		uselib = 'CORE SDL GL PNG')
	ctx.new_task_gen(
		features = 'cxx',
		source = ctx.path.ant_glob('lipsofsuna/image/*.cpp'),
		target = 'imgcxx_objs',
		uselib = 'CORE SDL GL SQUISH')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/main/*.c', excl=['lipsofsuna/main/main.c']),
		target = 'mai_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/math/*.c', excl=['lipsofsuna/math/unittest.c']),
		target = 'mat_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/model/*.c'),
		target = 'mdl_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/network/*.c'),
		target = 'net_objs',
		uselib = 'CORE GRAPPLE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/particle/*.c'),
		target = 'par_objs',
		uselib = 'CORE GL GLEW SDL')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/paths/*.c'),
		target = 'pth_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cxx',
		source = ctx.path.ant_glob('lipsofsuna/physics/*.cpp'),
		target = 'phy_objs',
		uselib = 'CORE BULLET')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/reload/*.c', excl=['lipsofsuna/reload/reload-main.c']),
		target = 'rel_objs',
		uselib = 'CORE SDL PNG SQUISH')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/render/*.c'),
		target = 'ren_objs',
		uselib = 'CORE SDL GLEW GL')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/script/*.c'),
		target = 'scr_objs',
		uselib = 'CORE LUA')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/server/*.c', excl=['lipsofsuna/server/server-main.c']),
		target = 'ser_objs',
		uselib = 'CORE LUA GRAPPLE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/sound/*.c'),
		target = 'snd_objs',
		uselib = 'CORE AL VORBIS OGG FLAC')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/string/*.c'),
		target = 'str_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/system/*.c'),
		target = 'sys_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/thread/*.c'),
		target = 'thr_objs',
		uselib = 'CORE THREAD')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/video/*.c'),
		target = 'vid_objs',
		uselib = 'CORE SDL GLEW GL')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/viewer/*.c', excl=['lipsofsuna/viewer/viewer-main.c']),
		target = 'vie_objs',
		uselib = 'CORE SDL GLEW GL')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/voxel/*.c'),
		target = 'vox_objs',
		uselib = 'CORE')
	ctx.new_task_gen(
		features = 'cc',
		source = ctx.path.ant_glob('lipsofsuna/widget/*.c'),
		target = 'wdg_objs',
		uselib = 'CORE SDL GLEW GL')
	ctx.new_task_gen(
		features = 'cc cprogram',
		source = 'lipsofsuna/main/main.c',
		target = 'lipsofsuna-bin',
		install_path = None,
		add_objects = 'sys_objs',
		uselib = 'CORE THREAD')
	ctx.set_group("install")
	instpath = os.path.join(ctx.env.BINDIR, 'lipsofsuna' + ctx.env.EXEEXT)
	ctx.install_as(instpath, 'lipsofsuna-bin' + ctx.env.EXEEXT, chmod = 0777)
	ctx.set_group("build")
	if ctx.env.SERVER:
		ctx.new_task_gen(
			features = 'cc cxx cprogram',
			source = 'lipsofsuna/server/server-main.c',
			target = 'lipsofsuna-server',
			install_path = ctx.env.PROGDIR,
			add_objects = 'ai_objs alg_objs arc_objs cal_objs eng_objs gen_objs mai_objs mat_objs mdl_objs net_objs phy_objs pth_objs scr_objs ser_objs str_objs sys_objs thr_objs vox_objs',
			uselib = 'CORE LUA SQLITE BULLET GRAPPLE ZLIB THREAD')
	if ctx.env.CLIENT:
		ctx.new_task_gen(
			features = 'cc cxx cprogram',
			source = 'lipsofsuna/client/client-main.c',
			target = 'lipsofsuna-client',
			install_path = ctx.env.PROGDIR,
			add_objects = 'ai_objs alg_objs arc_objs bnd_objs cal_objs cli_objs eng_objs fnt_objs gen_objs img_objs imgcxx_objs mai_objs mat_objs mdl_objs net_objs par_objs phy_objs pth_objs rel_objs ren_objs scr_objs ser_objs snd_objs str_objs sys_objs thr_objs vid_objs vox_objs wdg_objs',
			uselib = 'CORE LUA SQLITE BULLET GRAPPLE SDL SDL_TTF ZLIB GLEW GL PNG SQUISH THREAD AL VORBIS OGG FLAC')
	if ctx.env.GENERATOR:
		ctx.new_task_gen(
			features = 'cc cxx cprogram',
			source = 'lipsofsuna/generator/generator-main.c',
			target = 'lipsofsuna-generator',
			install_path = ctx.env.PROGDIR,
			add_objects = 'ai_objs alg_objs arc_objs cal_objs eng_objs gen_objs mai_objs mat_objs mdl_objs net_objs phy_objs pth_objs scr_objs ser_objs str_objs sys_objs thr_objs vox_objs',
			uselib = 'CORE LUA SQLITE BULLET GRAPPLE ZLIB THREAD')
	if ctx.env.IMPORT:
		ctx.new_task_gen(
			features = 'cc cxx cprogram',
			source = 'lipsofsuna/reload/reload-main.c',
			target = 'lipsofsuna-import',
			install_path = ctx.env.PROGDIR,
			add_objects = 'alg_objs arc_objs img_objs imgcxx_objs mat_objs mdl_objs pth_objs rel_objs str_objs sys_objs thr_objs',
			uselib = 'CORE SQLITE ZLIB PNG SDL GL GLEW SQUISH THREAD')
	if ctx.env.VIEWER:
		ctx.new_task_gen(
			features = 'cc cxx cprogram',
			source = 'lipsofsuna/viewer/viewer-main.c',
			target = 'lipsofsuna-viewer',
			install_path = ctx.env.PROGDIR,
			add_objects = 'ai_objs alg_objs arc_objs cal_objs eng_objs img_objs imgcxx_objs mai_objs mat_objs mdl_objs par_objs phy_objs pth_objs rel_objs ren_objs scr_objs str_objs sys_objs thr_objs vid_objs vie_objs vox_objs',
			uselib = 'CORE LUA SQLITE BULLET SDL SDL_TTF ZLIB GLEW GL PNG SQUISH THREAD')
	def buildext(ctx, pth, suf, use):
		dirs = ctx.path.ant_glob(pth + '*', excl=[pth + '.*'], dir=True, src=False, bld=False)
		for ext in dirs.replace(pth, '').split(' '):
			srcs = ctx.path.ant_glob(os.path.join(pth, ext, '*.c'))
			if srcs:
				ctx.new_task_gen(
					features = 'cc cshlib',
					source = srcs,
					target = ext + suf,
					install_path = ctx.env.EXTSDIR,
					uselib = use)
	if ctx.env.CLIENT or ctx.env.SERVER:
		buildext(ctx, 'lipsofsuna/extension/common/', '', 'EXTENSION LUA SQLITE GRAPPLE ZLIB THREAD')
	if ctx.env.CLIENT:
		buildext(ctx, 'lipsofsuna/extension/client/', '-cli', 'EXTENSION LUA SQLITE GRAPPLE SDL SDL_TTF ZLIB GLEW GL PNG THREAD AL VORBIS OGG FLAC')
	if ctx.env.SERVER:
		buildext(ctx, 'lipsofsuna/extension/server/', '-ser', 'EXTENSION LUA SQLITE GRAPPLE ZLIB THREAD')
	ctx.set_group("install")
	ctx.install_files(ctx.env.TOOLDIR, ['lipsofsuna/reload/blender-export.py'])

def html(ctx):
	Utils.exec_command('docs/makedoc.sh ' + VERSION)
