#! /usr/bin/env python
# encoding: utf-8

import os
import string
from waflib import Logs
from waflib import Options

APPNAME='lipsofsuna'
VERSION='0.6.0'

top = '.'
out = '.build'

CORE_DIRS = 'ai algorithm archive callback client engine extension generator main math model network particle paths physics script sound system'
CORE_DIRS_GFX = 'render render/font render/image render/internal render/overlay'
EXTS_DIRS = 'ai animation config-file database file filter heightmap heightmap-physics image image-merge lobby math model model-editing model-merge network noise object password physics physics-object physics-terrain reload sectors skeleton sound string terrain thread time vision watchdog'
EXTS_DIRS_GFX = 'camera graphics heightmap-render input render render-model render-object widgets wireframe'
LODT_DIRS = 'algorithm archive model system'

def options(opt):
	opt.load('compiler_c compiler_cxx')
	gr = opt.get_option_group('configure options')
	gr.add_option('--bindir', action='store', default=None, help='override executable directory [default: PREFIX/bin]')
	gr.add_option('--libdir', action='store', default=None, help='override library directory [default: PREFIX/lib]')
	gr.add_option('--datadir', action='store', default=None, help='override data directory [default: PREFIX/share]')
	gr.add_option('--ogre-plugindir', action='store', default=False, help='override the default Ogre plugin directory')
	gr.add_option('--disable-relpath', action='store_false', default=True, dest='relpath', help='disable relative data and library paths')
	gr.add_option('--disable-graphics', action='store_false', default=True, dest='graphics', help='compile without graphics support')
	gr.add_option('--disable-sound', action='store_false', default=True, dest='sound', help='compile without sound support')
	gr.add_option('--disable-luajit', action='store_false', default=True, dest='luajit', help='disable compilation against LuaJIT')
	gr.add_option('--disable-input-grabs', action='store_false', default=True, dest='grabs', help='disable input grabbing for debug purposes [default: false]')
	gr.add_option('--enable-optimization', action='store_true', default=False, dest='optimize', help='compile with heavy optimizations')
	gr.add_option('--enable-memdebug', action='store_true', default=False, dest='memdebug', help='compile with expensive memory debugging')

def configure(conf):

	# Usage.
	if Options.options.bindir or Options.options.libdir or Options.options.datadir:
		if Options.options.relpath:
			conf.fatal("The directory overrides are not used by a relative path build.\n" +\
			           "To enable a traditional build, configure with --relpath=false\n")

	# Flags.
	conf.load('compiler_c compiler_cxx')
	conf.env.INCLUDES_CORE = ['.', 'src']
	conf.env.INCLUDES_TEST = []
	conf.env.CFLAGS_CORE = conf.env.CFLAGS
	conf.env.CFLAGS_CORE.extend(['-g', '-Wall', '-DHAVE_CONFIG_H'])
	if Options.options.optimize:
		conf.env.CFLAGS_CORE.extend(['-O3'])
	conf.env.CFLAGS_TEST = conf.env.CFLAGS
	conf.env.CXXFLAGS_CORE = conf.env.CFLAGS_CORE
	conf.env.CXXFLAGS_TEST = conf.env.CFLAGS_TEST
	conf.env.LIBPATH_CORE = []
	conf.env.LIBPATH_TEST = []
	conf.env.LINKFLAGS_CORE = conf.env.LINKFLAGS
	conf.env.LINKFLAGS_CORE.extend(['-g'])
	conf.env.LINKFLAGS_TEST = conf.env.LINKFLAGS

	# Dependencies.
	conf.check(header_name='dlfcn.h', define_name='HAVE_DLFCN_H', mandatory=False)
	conf.check(header_name='fcntl.h', define_name='HAVE_FCNTL_H', mandatory=False)
	conf.check(header_name='inotifytools/inotify.h', define_name='HAVE_INOTIFYTOOLS_INOTIFY_H', mandatory=False)
	conf.check(header_name='inttypes.h', define_name='HAVE_INTTYPES_H', mandatory=False)
	conf.check(header_name='poll.h', define_name='HAVE_POLL_H', mandatory=False)
	conf.check(header_name='stdint.h', define_name='HAVE_STDINT_H', mandatory=False)
	conf.check(header_name='sys/inotify.h', define_name='HAVE_SYS_INOTIFY_H', mandatory=False)
	conf.check(header_name='sys/mman.h', define_name='HAVE_SYS_MMAN_H', mandatory=False)
	conf.check(header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
	conf.check(header_name='sys/time.h', define_name='HAVE_SYS_TIME_H')
	conf.check(header_name='sys/wait.h', define_name='HAVE_SYS_WAIT_H', mandatory=False)
	conf.check(header_name='time.h', define_name='HAVE_TIME_H')
	conf.check(header_name='unistd.h', define_name='HAVE_UNISTD_H')
	conf.check(header_name='windows.h', define_name='HAVE_WINDOWS_H', mandatory=False)
	conf.check(header_name='iconv.h', define_name='HAVE_ICONV_H')
	conf.check_cc(msg='Checking for function fork', header_name='unistd.h', function_name='fork', define_name='HAVE_FORK', mandatory=False)
	conf.check_cc(msg='Checking for function usleep', header_name='unistd.h', function_name='usleep', define_name='HAVE_USLEEP', mandatory=False)
	conf.check_cc(lib='dl', uselib_store='CORE', mandatory=False)
	conf.check_cc(lib='m', uselib_store='CORE', mandatory=False)
	conf.check_cc(lib='pthread', uselib_store='THREAD', mandatory=False)
	conf.check_cc(lib='iconv', uselib_store='CORE', mandatory=False) # BSD
	conf.check_zlib()
	conf.check_sqlite()
	conf.check_bullet()
	conf.check_enet()
	conf.check_lua()
	if not conf.check_freeimage():
		conf.check_libpng()
	if Options.options.graphics:
		conf.check_ogre()
		conf.check_ogre_plugindir()
		conf.check_ogre_terrain()
		conf.check_ois()
		conf.check_xlib()
	if Options.options.sound:
		conf.check_openal()
		conf.check_ogg()
		conf.check_vorbisfile()
		conf.check_flac()
	conf.check_curl()

	# Defines and paths.
	conf.define('LI_VERSION_STRING', VERSION)
	conf.define('LI_ENABLE_ERROR', 1)
	if not Options.options.grabs:
		conf.define('LI_DISABLE_INPUT_GRABS', 1)
	if not Options.options.graphics:
		conf.define('LI_DISABLE_GRAPHICS', 1)
	if not Options.options.sound:
		conf.define('LI_DISABLE_SOUND', 1)
	if Options.options.memdebug:
		conf.define('LI_ENABLE_MEMDEBUG', 1)
	if Options.options.relpath:
		conf.define('LI_RELATIVE_PATHS', 1)
		conf.env.RPATH_CORE = ['$ORIGIN/lib']
		conf.env.PREFIX = conf.path.abspath()
		conf.env.BINDIR = conf.env.PREFIX
		conf.env.EXTSDIR = os.path.join(conf.env.PREFIX, 'lib', 'extensions')
		conf.env.DATADIR = os.path.join(conf.env.PREFIX, 'data')
		conf.env.PROGDIR = os.path.join(conf.env.PREFIX, 'bin')
		conf.env.TOOLDIR = os.path.join(conf.env.PREFIX, 'tool')
	else:
		bindir = Options.options.bindir
		if not bindir:
			bindir = os.path.join(conf.env.PREFIX, 'bin')
		libdir = Options.options.libdir
		if not libdir:
			libdir = os.path.join(conf.env.PREFIX, 'lib')
		datadir = Options.options.datadir
		if not datadir:
			datadir = os.path.join(conf.env.PREFIX, 'share')
		conf.env.BINDIR = bindir
		conf.env.EXTSDIR = os.path.join(libdir, APPNAME, 'extensions', '0.1')
		conf.env.DATADIR = os.path.join(datadir, APPNAME)
		conf.env.PROGDIR = bindir
		conf.env.TOOLDIR = os.path.join(datadir, APPNAME, 'tool')
		conf.env.APPDIR = os.path.join(datadir, 'applications')
		conf.env.ICONDIR = os.path.join(datadir, 'pixmaps')
		conf.define('LIEXTSDIR', conf.env.EXTSDIR)
		conf.define('LIDATADIR', os.path.join(datadir, APPNAME))
		conf.define('LIPROGDIR', conf.env.PROGDIR)
		conf.define('LITOOLDIR', conf.env.TOOLDIR)
	if conf.env.OGRE_plugindir:
		conf.define('OGRE_PLUGIN_DIR', conf.env.OGRE_plugindir)
	conf.write_config_header('config.h')

	# Messages.
	print("\nConfigured with:")
	if Options.options.relpath:
		print("\trelative paths")
	else:
		print("\tbindir: " + bindir)
		print("\tlibdir: " + libdir)
		print("\tdatadir: " + datadir)
	if Options.options.graphics:
		print("\tgraphics support")
	if Options.options.sound:
		print("\tsound support")
	if conf.env.CURL:
		print("\tmaster server connectivity")
	if conf.env.FREEIMAGE:
		print("\timage loading with FreeImage")
	if conf.env.LIBPNG:
		print("\image loading with libpng")
	if Options.options.memdebug:
		print("\tmemory debugging")
	if conf.env.OGRE_plugindir:
		print("\tOgre plugin directory: " + conf.env.OGRE_plugindir)
	print("\nBuild command: ./waf")
	print("Install command: ./waf install\n")

def build(bld):
	def get_dirs(core, gfx):
		dirs = core.split(' ')
		if Options.options.graphics:
			dirs += gfx.split(' ')
		return dirs
	bld.add_group("build")
	bld.add_group("install")
	bld.set_group("build")
	# Game.
	srcs = []
	for dir in get_dirs(CORE_DIRS, CORE_DIRS_GFX):
		srcs += bld.path.ant_glob('src/lipsofsuna/%s/*.c' % dir)
		srcs += bld.path.ant_glob('src/lipsofsuna/%s/*.cpp' % dir)
	for dir in get_dirs(EXTS_DIRS, EXTS_DIRS_GFX):
		srcs += bld.path.ant_glob('src/lipsofsuna/extension/%s/*.c' % dir)
		srcs += bld.path.ant_glob('src/lipsofsuna/extension/%s/*.cpp' % dir)
	bld.program(
		target = 'lipsofsuna',
		source = srcs,
		install_path = '${BINDIR}',
		use = 'CORE LUA SQLITE BULLET ENET OIS OGRE PNG FREEIMAGE THREAD AL VORBIS OGG FLAC CURL ZLIB')
	# LOD tool.
	srcs = list(bld.path.ant_glob('src/lodtool/*.c'))
	for dir in LODT_DIRS.split(' '):
		srcs += bld.path.ant_glob('src/lipsofsuna/%s/*.c' % dir)
	bld.program(
		target = 'lipsofsuna-lodtool',
		source = srcs,
		install_path = '${BINDIR}',
		use = 'CORE THREAD ZLIB')
	# Installation.
	bld.set_group("install")
	if not Options.options.relpath:
		start_dir = bld.path.find_dir('tool')
		bld.install_files(bld.env.TOOLDIR, start_dir.ant_glob('lipsofsuna_export/*.py'), cwd=start_dir, relative_trick=True)
		start_dir = bld.path.find_dir('data')
		bld.install_files('${DATADIR}', start_dir.ant_glob('**/*.*'), cwd=start_dir, relative_trick=True)
		bld.install_files('${APPDIR}', ['misc/lipsofsuna.desktop'])
		bld.install_files('${ICONDIR}', ['misc/lipsofsuna.png', 'misc/lipsofsuna.svg'])

def dist(ctx):
	import tarfile
	dirs = ['src/**/*.*', 'data/**/*.*', 'tool/*', 'misc/*', 'docs/*', 'AUTHORS', 'COPYING', 'NEWS', 'README', 'waf', 'wscript']
	excl = ['**/.*', '**/import', 'docs/html']
	base = APPNAME + '-' + VERSION
	name = base + '.tar.gz'
	Logs.pprint('GREEN', "Creating `%s'" % name)
	tar = tarfile.open(name, 'w:gz')
	for f in ctx.path.ant_glob(dirs, excl=excl):
		tinfo = tar.gettarinfo(name = f.abspath(), arcname = base + '/' + f.path_from(ctx.path))
		tinfo.uid = 0
		tinfo.gid = 0
		tinfo.uname = 'root'
		tinfo.gname = 'root'
		fu = None
		try:
			fu = open(f.abspath())
			tar.addfile(tinfo, fileobj = fu)
		finally:
			fu.close()
	tar.close()
	Logs.pprint('GREEN', 'Done')
	exit()

def html(ctx):
	import re
	import shutil
	# Initialize directories.
	luadir = os.path.join(ctx.path.abspath(), 'docs', 'html')
	shutil.rmtree(luadir, True)
	os.makedirs(luadir)
	# Compile the Doxygen documentation.
	ctx.exec_command('doxygen docs/Doxyfile')
	Logs.pprint('GREEN', "Built Doxygen documentation")
	# Compile the script documentation.
	orig_dir = os.getcwd()
	os.chdir(os.path.join(ctx.path.abspath(), 'data', 'system'))
	ctx.exec_command('ldoc.lua .')
	os.chdir(os.path.join(ctx.path.abspath(), 'data', 'unittest'))
	ctx.exec_command('ldoc.lua .')
	os.chdir(os.path.join(ctx.path.abspath(), 'data', 'lipsofsuna'))
	ctx.exec_command('ldoc.lua .')
	os.chdir(orig_dir)
	Logs.pprint('GREEN', "Built Lua documentation")

##############################################################################
# Configuration checks.

from waflib.Configure import conf

@conf
def check_zlib(conf):
	conf.start_msg('Checking for package zlib')
	conf.env.stash()
	if conf.check_cfg(package='zlib', atleast_version='1.2.0', args='--cflags --libs', mandatory=False):
		conf.end_msg('pkg-config zlib')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='z', mandatory=False, uselib_store='ZLIB') and\
	   conf.check_cc(header_name='zlib.h', mandatory=False, uselib_store='ZLIB'):
		conf.end_msg('library z')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='zdll', mandatory=False, uselib_store='ZLIB') and\
	   conf.check_cc(header_name='zlib.h', mandatory=False, uselib_store='ZLIB'):
		conf.end_msg('library zdll')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find zlib')

@conf
def check_sqlite(conf):
	conf.start_msg('Checking for package sqlite')
	conf.env.stash()
	if conf.check_cfg(package='sqlite3', atleast_version='3.6.0', args='--cflags --libs', uselib_store='SQLITE', mandatory=False):
		conf.end_msg('pkg-config sqlite3')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='sqlite3', mandatory=False, uselib='TEST', uselib_store='SQLITE') and\
	   conf.check_cc(header_name='sqlite3.h', mandatory=False, uselib='TEST', uselib_store='SQLITE'):
		conf.end_msg('library sqlite3')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find sqlite')

@conf
def check_bullet(conf):
	conf.start_msg('Checking for package bullet')
	conf.env.stash()
	if conf.check_cfg(package='bullet', atleast_version='2.77', args='--cflags --libs', mandatory=False):
		conf.end_msg('pkg-config bullet')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cxx(lib='linearmath', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(lib='bulletcollision', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(lib='bulletdynamics', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(header_name='btBulletCollisionCommon.h', mandatory=False, uselib='TEST BULLET', uselib_store='BULLET'):
		conf.env.LINKFLAGS_BULLET = ['-llinearmath', '-lbulletcollision', '-lbulletdynamics']
		conf.end_msg('libraries linearmath bulletcollision bulletdynamics')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cxx(lib='LinearMath', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(lib='BulletCollision', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(lib='BulletDynamics', uselib='TEST', mandatory=False) and\
	   conf.check_cxx(header_name='btBulletCollisionCommon.h', mandatory=False, uselib='TEST BULLET', uselib_store='BULLET'):
		conf.env.LINKFLAGS_BULLET = ['-lLinearMath', '-lBulletCollision', '-lBulletDynamics']
		conf.end_msg('libraries LinearMath BulletCollision BulletDynamics')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find bullet')

@conf
def check_enet(conf):
	conf.start_msg('Checking for package enet')
	conf.env.stash()
	if conf.check_cfg(package='enet', atleast_version='1.2.0', args='--cflags --libs', uselib_store='ENET', mandatory=False):
		conf.end_msg('pkg-config enet')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='libenet', atleast_version='1.2.0', args='--cflags --libs', uselib_store='ENET', mandatory=False):
		conf.end_msg('pkg-config libenet')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='enet/enet.h', mandatory=False, uselib='TEST', uselib_store='ENET') and\
	   conf.check_cc(lib='enet', mandatory=False, uselib='TEST', uselib_store='ENET'):
		conf.end_msg('library enet')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find enet')

@conf
def check_lua(conf):
	conf.start_msg('Checking for package lua')
	if Options.options.luajit:
		# Ubuntu Lucid ships a broken 32-bit LuaJIT on 64-bit systems so we need some extra
		# checks to know if it really works or if we should fall back to standard Lua.
		conf.env.stash()
		if conf.check_cfg(package='luajit', atleast_version='2.0.0', args='--cflags --libs', uselib_store='LUA', mandatory=False) and\
	       conf.check(fragment='int main() { return 0; }\n', mandatory=False, uselib='TEST LUA'):
			conf.end_msg('pkg-config luajit')
			return
		conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='lua', atleast_version='5.1', args='--cflags --libs', uselib_store='LUA', mandatory=False):
		conf.end_msg('pkg-config lua')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='lua5.1', atleast_version='5.1', args='--cflags --libs', uselib_store='LUA', mandatory=False):
		conf.end_msg('pkg-config lua5.1')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='lua5.2', atleast_version='5.2', args='--cflags --libs', uselib_store='LUA', mandatory=False):
		conf.end_msg('pkg-config lua5.2')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='lua.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(header_name='lauxlib.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(lib='lua', mandatory=False, uselib='TEST', uselib_store='LUA'):
		conf.end_msg('library lua')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='lua.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(header_name='lauxlib.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(lib='lua5.1', uselib='TEST', uselib_store='LUA', mandatory=False):
		conf.end_msg('library lua5.1')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='lua.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(header_name='lauxlib.h', mandatory=False, uselib='TEST', uselib_store='LUA') and\
	   conf.check_cc(lib='lua5.2', uselib='TEST', uselib_store='LUA', mandatory=False):
		conf.end_msg('library lua5.2')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find lua')

@conf
def check_freeimage(conf):
	conf.start_msg('Checking for package FreeImage')
	conf.env.stash()
	if conf.check_cfg(package='freeimage', atleast_version='3.0.0', args='--cflags --libs', uselib_store='FREEIMAGE', mandatory=False):
		conf.env.FREEIMAGE = True
		conf.define('HAVE_FREEIMAGE', 1)
		conf.end_msg('pkg-config freeimage')
		return True
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='freeimage', mandatory=False, uselib_store='FREEIMAGE') and\
	   conf.check_cc(header_name='FreeImage.h', mandatory=False, uselib_store='FREEIMAGE'):
		conf.env.FREEIMAGE = True
		conf.define('HAVE_FREEIMAGE', 1)
		conf.end_msg('library freeimage')
		return True
	conf.env.revert()
	conf.end_msg('not found, trying libpng instead', 'YELLOW')
	return False

@conf
def check_libpng(conf):
	conf.start_msg('Checking for package libpng')
	conf.env.stash()
	if conf.check_cfg(package='libpng', atleast_version='1.2.0', args='--cflags --libs', uselib_store='PNG', mandatory=False):
		conf.env.LIBPNG = True
		conf.define('HAVE_LIBPNG', 1)
		conf.end_msg('pkg-config libpng')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='libpng12', atleast_version='1.2.0', args='--cflags --libs', uselib_store='PNG', mandatory=False):
		conf.env.LIBPNG = True
		conf.define('HAVE_LIBPNG', 1)
		conf.end_msg('pkg-config libpng12')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='libpng14', atleast_version='1.4.0', args='--cflags --libs', uselib_store='PNG', mandatory=False):
		conf.env.LIBPNG = True
		conf.define('HAVE_LIBPNG', 1)
		conf.end_msg('pkg-config libpng14')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='png', mandatory=False, uselib_store='PNG') and\
	   conf.check_cc(header_name='png.h', mandatory=False, uselib_store='PNG'):
		conf.env.LIBPNG = True
		conf.define('HAVE_LIBPNG', 1)
		conf.end_msg('library png')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find freeimage or libpng')
	return False

@conf
def check_ogre(conf):
	conf.start_msg('Checking for package OGRE')
	conf.env.stash()
	if conf.check_cfg(package='OGRE', atleast_version='1.7.0', args='--cflags --libs', mandatory=False):
		conf.end_msg('pkg-config OGRE')
		return
	conf.env.revert()
	conf.env.stash()
	# Some kind of a bug seems to occur with MinGW 4.6 as the linker won't
	# output anything if the main function does not contain a function call.
	if conf.check_cxx(lib='OgreMain', mandatory=False, uselib='TEST', uselib_store='OGRE') and\
	   conf.check_cxx(header_name='Ogre.h', mandatory=False, uselib='TEST', uselib_store='OGRE', fragment='''
			#include <stdio.h>
			int main() { printf(""); return 0; }'''):
		conf.end_msg('library OgreMain')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find OGRE')

@conf
def check_ogre_plugindir(conf):
	conf.start_msg('Checking for OGRE plugindir')
	if Options.options.ogre_plugindir:
		conf.env.OGRE_plugindir = Options.options.ogre_plugindir
		conf.end_msg('argument %s' % conf.env.OGRE_plugindir)
		return
	conf.env.stash()
	tmp = conf.check_cfg(package='OGRE', args='--variable=plugindir', mandatory=False, uselib_store='TMP')
	conf.env.revert()
	if tmp:
		conf.env.OGRE_plugindir = tmp.strip()
		conf.end_msg('pkg-config %s' % conf.env.OGRE_plugindir)
		return
	conf.env.revert()
	conf.end_msg(False)

@conf
def check_ogre_terrain(conf):
	conf.start_msg('Checking for package OGRE-Terrain')
	conf.env.stash()
	if conf.check_cfg(package='OGRE-Terrain', atleast_version='1.7.0', args='--cflags --libs', mandatory=False, uselib_store='OGRE'):
		conf.end_msg('pkg-config OGRE-Terrain')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cxx(lib='OgreTerrain', mandatory=False, uselib='TEST', uselib_store='OGRE') and\
	   conf.check_cxx(header_name='Terrain/OgreTerrain.h', mandatory=False, uselib='TEST', uselib_store='OGRE', fragment='''
			#include <stdio.h>
			int main() { printf(""); return 0; }'''):
		conf.end_msg('library OgreTerrain')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find OGRE-Terrain')

@conf
def check_ois(conf):
	conf.start_msg('Checking for package OIS')
	conf.env.stash()
	if conf.check_cfg(package='OIS', atleast_version='1.3.0', args='--cflags --libs', mandatory=False):
		conf.end_msg('pkg-config OIS')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cxx(lib='OIS', mandatory=False, uselib='TEST', uselib_store='OIS') and\
	   conf.check_cxx(header_name='OIS.h', mandatory=False, uselib='TEST', uselib_store='OIS'):
		conf.end_msg('library OIS')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find OIS')
	   
@conf
def check_xlib(conf):
	# Nice to have because Ogre can't change the window title.
	conf.start_msg('Checking for package Xlib')
	conf.env.stash()
	if conf.check_cc(lib='X11', uselib_store='OGRE', mandatory=False, uselib='TEST') and\
	   conf.check_cc(header_name='X11/Xlib.h', uselib_store='OGRE', mandatory=False, uselib='TEST', define_name='HAVE_X11_XLIB_H'):
		conf.define('HAVE_XLIB', 1)
		conf.end_msg('library X11')
		return
	conf.env.revert()
	conf.end_msg(False)

@conf
def check_openal(conf):
	conf.start_msg('Checking for package openal')
	conf.env.stash()
	if conf.check_cfg(package='openal', atleast_version='0.0.8', args='--cflags --libs', uselib_store="AL", mandatory=False):
		conf.end_msg('pkg-config openal')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='openal', mandatory=False, uselib='TEST', uselib_store='AL') and\
	   conf.check_cc(header_name='AL/al.h', mandatory=False, uselib='TEST', uselib_store='AL'):
		conf.end_msg('library openal')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='OpenAL32', mandatory=False, uselib='TEST', uselib_store='AL') and\
	   conf.check_cc(header_name='AL/al.h', mandatory=False, uselib='TEST', uselib_store='AL'):
		conf.end_msg('library OpenAL32')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find openal')

@conf
def check_ogg(conf):
	conf.start_msg('Checking for package ogg')
	conf.env.stash()
	if conf.check_cfg(package='ogg', atleast_version='1.1.0', args='--cflags --libs', mandatory=False):
		conf.end_msg('pkg-config ogg')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='ogg', mandatory=False, uselib='TEST', uselib_store='OGG') and\
	   conf.check_cc(header_name='ogg.h', mandatory=False, uselib='TEST', uselib_store='OGG'):
		conf.end_msg('library ogg')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find ogg')

@conf
def check_vorbisfile(conf):
	conf.start_msg('Checking for package vorbisfile')
	conf.env.stash()
	if conf.check_cfg(package='vorbisfile', atleast_version='1.2.0', args='--cflags --libs', uselib_store='VORBIS', mandatory=False):
		conf.end_msg('pkg-config vorbisfile')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='vorbisfile', mandatory=False, uselib='TEST', uselib_store='VORBIS') and\
	   conf.check_cc(header_name='vorbisfile.h', mandatory=False, uselib='TEST', uselib_store='VORBIS'):
		conf.end_msg('library vorbisfile')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find vorbisfile')

@conf
def check_flac(conf):
	conf.start_msg('Checking for package flac')
	conf.env.stash()
	if conf.check_cfg(package='flac', atleast_version='1.2.0', args='--cflags --libs', uselib_store='FLAC', mandatory=False):
		conf.check_cc(header_name='stream_decoder.h', mandatory=False, uselib='TEST FLAC', uselib_store='FLAC')
		conf.check_cc(header_name='FLAC/stream_decoder.h', mandatory=False, uselib='TEST FLAC', uselib_store='FLAC')
		conf.end_msg('pkg-config flac')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='stream_decoder.h', mandatory=False, uselib='TEST', uselib_store='FLAC') and\
	   conf.check_cc(lib='FLAC', mandatory=False, uselib='TEST', uselib_store='FLAC'):
		conf.end_msg('library FLAC')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(header_name='FLAC/stream_decoder.h', mandatory=False, uselib='TEST', uselib_store='FLAC') and\
	   conf.check_cc(lib='FLAC', mandatory=False, uselib='TEST', uselib_store='FLAC'):
		conf.end_msg('library FLAC')
		return
	conf.env.revert()
	conf.end_msg(False)
	conf.fatal('Could not find flac')

@conf
def check_curl(conf):
	conf.start_msg('Checking for package curl')
	conf.env.stash()
	if conf.check_cfg(package='libcurl', args='--cflags --libs', uselib_store="CURL", mandatory=False):
		conf.env.CURL = True
		conf.define('HAVE_CURL', 1)
		conf.end_msg('pkg-config libcurl')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cfg(package='curl', args='--cflags --libs', uselib_store="CURL", mandatory=False):
		conf.env.CURL = True
		conf.define('HAVE_CURL', 1)
		conf.end_msg('pkg-config curl')
		return
	conf.env.revert()
	conf.env.stash()
	if conf.check_cc(lib='curl', mandatory=False, uselib='TEST', uselib_store='CURL') and\
	   conf.check_cc(header_name='curl/curl.h', lib='curl', mandatory=False, uselib='TEST', uselib_store='CURL'):
		conf.env.CURL = True
		conf.define('HAVE_CURL', 1)
		conf.end_msg('library curl')
		return
	conf.env.revert()
	conf.end_msg('not found, disabling master server connection', 'YELLOW')
