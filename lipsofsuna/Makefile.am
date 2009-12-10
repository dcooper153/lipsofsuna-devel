SUBDIRS = .
if MAYBE_CLIENT
SUBDIRS += client/extensions
endif
if MAYBE_SERVER
SUBDIRS += server/extensions
endif

AM_CFLAGS = $(LICLI_CFLAGS)
AM_CXXFLAGS = $(CXXFLAGS) $(LICLI_CFLAGS)
INCLUDES = -I$(top_srcdir)
EXTRA_DIST = math/unittest.c reload/blender-export.py

editordir = $(TOOLDIR)
editor_DATA = reload/blender-export.py

prgdir = $(PROGDIR)
prg_PROGRAMS = lipsofsuna
bin_PROGRAMS =
lib_LTLIBRARIES =
if MAYBE_CLIENT
bin_PROGRAMS += lipsofsuna-client
lib_LTLIBRARIES += liblipsofsunaclient.la
endif
if MAYBE_GENERATOR
bin_PROGRAMS += lipsofsuna-generator
lib_LTLIBRARIES += liblipsofsunagenerator.la
endif
if MAYBE_IMPORT
bin_PROGRAMS += lipsofsuna-import
lib_LTLIBRARIES += liblipsofsunaimport.la
endif
if MAYBE_SERVER
bin_PROGRAMS += lipsofsuna-server
lib_LTLIBRARIES += liblipsofsunaserver.la
endif
if MAYBE_VIEWER
bin_PROGRAMS += lipsofsuna-viewer
lib_LTLIBRARIES += liblipsofsunaviewer.la
endif

##############################################################################

aidir = $(includedir)/lipsofsuna/ai
ai_HEADERS = \
	ai/lips-ai.h \
	ai/ai-manager.h \
	ai/ai-path.h \
	ai/ai-sector.h \
	ai/ai-types.h \
	ai/ai-waypoint.h
aisrc = \
	ai/ai-manager.c \
	ai/ai-path.c \
	ai/ai-sector.c

algdir = $(includedir)/lipsofsuna/algorithm
alg_HEADERS = \
	algorithm/lips-algorithm.h \
	algorithm/algorithm-array.h \
	algorithm/algorithm-astar.h \
	algorithm/algorithm-bst.h \
	algorithm/algorithm-camera.h \
	algorithm/algorithm-list.h \
	algorithm/algorithm-memdic.h \
	algorithm/algorithm-octree.h \
	algorithm/algorithm-priority-queue.h \
	algorithm/algorithm-ptrdic.h \
	algorithm/algorithm-range.h \
	algorithm/algorithm-range-iter.h \
	algorithm/algorithm-strdic.h \
	algorithm/algorithm-u32dic.h
algsrc = \
	algorithm/algorithm-astar.c \
	algorithm/algorithm-bst.c \
	algorithm/algorithm-camera.c \
	algorithm/algorithm-octree.c

arcdir = $(includedir)/lipsofsuna/archive
arc_HEADERS = \
	archive/lips-archive.h \
	archive/archive-reader.h \
	archive/archive-sql.h \
	archive/archive-tar.h \
	archive/archive-writer.h
arcsrc = \
	archive/archive-reader.c \
	archive/archive-sql.c \
	archive/archive-tar.c \
	archive/archive-writer.c

bnddir = $(includedir)/lipsofsuna/binding
bnd_HEADERS = \
	binding/lips-binding.h \
	binding/binding.h \
	binding/binding-action.h \
	binding/binding-manager.h \
	binding/binding-types.h
bndsrc = \
	binding/binding.c \
	binding/binding-action.c \
	binding/binding-manager.c

caldir = $(includedir)/lipsofsuna/callback
cal_HEADERS = \
	callback/lips-callback.h \
	callback/cal-callbacks.h \
	callback/cal-marshal.h
calsrc = \
	callback/cal-callbacks.c \
	callback/cal-marshal.c

clidir = $(includedir)/lipsofsuna/client
cli_HEADERS = \
	client/lips-client.h \
	client/client.h \
	client/client-callbacks.h \
	client/client-extension.h \
	client/client-module.h \
	client/client-network.h \
	client/client-object.h \
	client/client-script.h \
	client/client-types.h \
	client/client-window.h \
	client/widget-login.h
clisrc = \
	client/client.c \
	client/client-callbacks.c \
	client/client-command.c \
	client/client-module.c \
	client/client-network.c \
	client/client-object.c \
	client/client-render.c \
	client/client-script-action.c \
	client/client-script-binding.c \
	client/client-script-group.c \
	client/client-script-extension.c \
	client/client-script-light.c \
	client/client-script-module.c \
	client/client-script-object.c \
	client/client-script-player.c \
	client/client-script-scene.c \
	client/client-script-widget.c \
	client/client-script-window.c \
	client/client-window.c \
	client/widget-login.c

cfgdir = $(includedir)/lipsofsuna/config
cfg_HEADERS = \
	config/lips-config.h \
	config/config-account.h \
	config/config-bans.h \
	config/config-host.h
cfgsrc = \
	config/config-account.c \
	config/config-bans.c \
	config/config-host.c

engdir = $(includedir)/lipsofsuna/engine
eng_HEADERS = \
	engine/lips-engine.h \
	engine/engine.h \
	engine/engine-constraint.h \
	engine/engine-iterator.h \
	engine/engine-model.h \
	engine/engine-object.h \
	engine/engine-resources.h \
	engine/engine-sector.h \
	engine/engine-selection.h \
	engine/engine-types.h
engsrc = \
	engine/engine.c \
	engine/engine-constraint.c \
	engine/engine-model.c \
	engine/engine-object.c \
	engine/engine-resources.c \
	engine/engine-sector.c \
	engine/engine-selection.c

fntdir = $(includedir)/lipsofsuna/font
fnt_HEADERS = \
	font/lips-font.h \
	font/font.h \
	font/font-layout.h
fntsrc = \
	font/font.c \
	font/font-layout.c

gendir = $(includedir)/lipsofsuna/generator
gen_HEADERS = \
	generator/lips-generator.h \
	generator/generator.h \
	generator/generator-brush.h \
	generator/generator-rule.h \
	generator/generator-types.h
gensrc = \
	generator/generator.c \
	generator/generator-brush.c \
	generator/generator-rule.c

imgdir = $(includedir)/lipsofsuna/image
img_HEADERS = \
	image/lips-image.h \
	image/image.h \
	image/image-compress.h \
	image/image-dds.h \
	image/image-texture.h
imgsrc = \
	image/image.c \
	image/image-compress.cpp \
	image/image-texture.c

maisrc = \
	main/main.c

matdir = $(includedir)/lipsofsuna/math
mat_HEADERS = \
	math/lips-math.h \
	math/math-aabb.h \
	math/math-convert.h \
	math/math-frustum.h  \
	math/math-generic.h \
	math/math-geometry.h \
	math/math-halton.h \
	math/math-intersect.h \
	math/math-matrix.h \
	math/math-plane.h \
	math/math-polygon.h \
	math/math-quadratic.h \
	math/math-quaternion.h \
	math/math-rectangle.h \
	math/math-transform.h \
	math/math-triangle.h \
	math/math-types.h \
	math/math-vector.h \
	math/math-vertex.h
matsrc = \
	math/math.c

mdldir = $(includedir)/lipsofsuna/model
mdl_HEADERS = \
	model/lips-model.h \
	model/model.h \
	model/model-animation.h \
	model/model-bone.h \
	model/model-faces.h \
	model/model-hair.h \
	model/model-light.h \
	model/model-material.h \
	model/model-node.h \
	model/model-pose.h \
	model/model-shape.h \
	model/model-texture.h \
	model/model-types.h \
	model/model-vertex.h
mdlsrc = \
	model/model.c \
	model/model-animation.c \
	model/model-bone.c \
	model/model-faces.c \
	model/model-hair.c \
	model/model-light.c \
	model/model-material.c \
	model/model-node.c \
	model/model-pose.c \
	model/model-shape.c \
	model/model-texture.c

netdir = $(includedir)/lipsofsuna/network
net_HEADERS = \
	network/lips-network.h \
	network/network-extension.h \
	network/network-protocol.h
netsrc = \
	network/network.c

pardir = $(includedir)/lipsofsuna/particle
par_HEADERS = \
	particle/lips-particle.h \
	particle/particle-line.h \
	particle/particle-manager.h \
	particle/particle-point.h
parsrc = \
	particle/particle-line.c \
	particle/particle-manager.c \
	particle/particle-point.c

phydir = $(includedir)/lipsofsuna/physics
phy_HEADERS = \
	physics/lips-physics.h \
	physics/physics.h \
	physics/physics-constraint.h \
	physics/physics-object.h \
	physics/physics-shape.h \
	physics/physics-types.h
physrc = \
	physics/physics.cpp \
	physics/physics-constraint.cpp \
	physics/physics-control.cpp \
	physics/physics-object.cpp \
	physics/physics-private.cpp \
	physics/physics-private.h \
	physics/physics-shape.cpp

pthdir = $(includedir)/lipsofsuna/path
pth_HEADERS = \
	paths/lips-paths.h \
	paths/paths.h
pthsrc = \
	paths/paths.c

reldir = $(includedir)/lipsofsuna/reload
rel_HEADERS = \
	reload/lips-reload.h \
	reload/reload.h
relsrc = \
	reload/reload.c \
	reload/reload-blender.c \
	reload/reload-gimp.c \
	reload/reload-image.c

rnddir = $(includedir)/lipsofsuna/render
rnd_HEADERS = \
	render/lips-render.h \
	render/render.h \
	render/render-buffer.h \
	render/render-context.h \
	render/render-deferred.h \
	render/render-draw.h \
	render/render-group.h \
	render/render-image.h \
	render/render-light.h \
	render/render-lighting.h \
	render/render-material.h \
	render/render-object.h \
	render/render-scene.h \
	render/render-shader.h \
	render/render-texture.h \
	render/render-types.h \
	render/render-uniform.h
rndsrc = \
	render/render.c \
	render/render-buffer.c \
	render/render-context.c \
	render/render-deferred.c \
	render/render-draw.c \
	render/render-group.c \
	render/render-image.c \
	render/render-light.c \
	render/render-lighting.c \
	render/render-material.c \
	render/render-model.c \
	render/render-model.h \
	render/render-object.c \
	render/render-scene.c \
	render/render-shader.c \
	render/render-texture.c

scrdir = $(includedir)/lipsofsuna/script
scr_HEADERS = \
	script/lips-script.h \
	script/script.h \
	script/script-class.h \
	script/script-data.h \
	script/script-event.h \
	script/script-library.h \
	script/script-packet.h \
	script/script-types.h \
	script/script-util.h
scrsrc = \
	script/script.c \
	script/script-class.c \
	script/script-data.c \
	script/script-event.c \
	script/script-library.c \
	script/script-object.c \
	script/script-packet.c \
	script/script-path.c \
	script/script-private.h \
	script/script-quaternion.c \
	script/script-util.c \
	script/script-vector.c

snddir = $(includedir)/lipsofsuna/sound
snd_HEADERS = \
	sound/lips-sound.h \
	sound/sound-manager.h \
	sound/sound-sample.h \
	sound/sound-source.h \
	sound/sound-system.h
sndsrc = \
	sound/sound-manager.c \
	sound/sound-sample.c \
	sound/sound-source.c \
	sound/sound-system.c

srvdir = $(includedir)/lipsofsuna/server
srv_HEADERS = \
	server/lips-server.h \
	server/server.h \
	server/server-callbacks.h \
	server/server-client.h \
	server/server-extension.h \
	server/server-network.h \
	server/server-object.h \
	server/server-observer.h \
	server/server-script.h \
	server/server-types.h
srvsrc = \
	server/server.c \
	server/server-callbacks.c \
	server/server-client.c \
	server/server-network.c \
	server/server-object.c \
	server/server-script-effect.c \
	server/server-script-extension.c \
	server/server-script-object.c \
	server/server-script-server.c

strdir = $(includedir)/lipsofsuna/string
str_HEADERS = \
	string/lips-string.h \
	string/string-generic.h
strsrc = \
	string/string-generic.c

sysdir = $(includedir)/lipsofsuna/system
sys_HEADERS = \
	system/lips-system.h \
	system/system.h \
	system/system-compiler.h \
	system/system-directory.h \
	system/system-endian.h \
	system/system-error.h \
	system/system-file.h \
	system/system-execute.h \
	system/system-memory.h \
	system/system-misc.h \
	system/system-module.h \
	system/system-notify.h \
	system/system-path.h \
	system/system-relative.h \
	system/system-types.h \
	system/system-user.h
syssrc = \
	system/system.c \
	system/system-directory.c \
	system/system-error.c \
	system/system-file.c \
	system/system-execute.c \
	system/system-memory.c \
	system/system-misc.c \
	system/system-module.c \
	system/system-notify.c \
	system/system-path.c \
	system/system-relative.c \
	system/system-types.c \
	system/system-user.c

thrdir = $(includedir)/lipsofsuna/thread
thr_HEADERS = \
	thread/lips-thread.h \
	thread/thread.h \
	thread/thread-async-call.h
thrsrc = \
	thread/thread.c \
	thread/thread-async-call.c

viddir = $(includedir)/lipsofsuna/video
vid_HEADERS = \
	video/lips-video.h \
	video/video-calls.h \
	video/video-features.h \
	video/video-opengl.h
vidsrc = \
	video/video-calls.c \
	video/video-features.c

viedir = $(includedir)/lipsofsuna/viewer
vie_HEADERS = \
	viewer/viewer.h
viesrc = \
	viewer/viewer.c

voxdir = $(includedir)/lipsofsuna/voxel
vox_HEADERS = \
	voxel/lips-voxel.h \
	voxel/voxel.h \
	voxel/voxel-block.h \
	voxel/voxel-iterator.h \
	voxel/voxel-manager.h \
	voxel/voxel-material.h \
	voxel/voxel-sector.h \
	voxel/voxel-types.h
voxsrc = \
	voxel/voxel.c \
	voxel/voxel-block.c \
	voxel/voxel-manager.c \
	voxel/voxel-material.c \
	voxel/voxel-private.h \
	voxel/voxel-sector.c

wdgdir = $(includedir)/lipsofsuna/widget
wdg_HEADERS = \
	widget/lips-widget.h \
	widget/widget.h \
	widget/widget-busy.h \
	widget/widget-button.h \
	widget/widget-check.h \
	widget/widget-class.h \
	widget/widget-container.h \
	widget/widget-entry.h \
	widget/widget-event.h \
	widget/widget-group.h \
	widget/widget-image.h \
	widget/widget-label.h \
	widget/widget-manager.h \
	widget/widget-menu.h \
	widget/widget-menugroup.h \
	widget/widget-progress.h \
	widget/widget-render.h \
	widget/widget-scroll.h \
	widget/widget-spin.h \
	widget/widget-style.h \
	widget/widget-tabs.h \
	widget/widget-tree.h \
	widget/widget-types.h \
	widget/widget-view.h \
	widget/widget-window.h
wdgsrc = \
	widget/widget.c \
	widget/widget-busy.c \
	widget/widget-button.c \
	widget/widget-check.c \
	widget/widget-container.c \
	widget/widget-entry.c \
	widget/widget-group.c \
	widget/widget-image.c \
	widget/widget-label.c \
	widget/widget-manager.c \
	widget/widget-menu.c \
	widget/widget-menugroup.c \
	widget/widget-private.h \
	widget/widget-progress.c \
	widget/widget-render.c \
	widget/widget-scroll.c \
	widget/widget-spin.c \
	widget/widget-style.c \
	widget/widget-tabs.c \
	widget/widget-tree.c \
	widget/widget-view.c \
	widget/widget-window.c

##############################################################################

liblipsofsunaclient_la_LDFLAGS = -no-undefined
liblipsofsunaclient_la_LIBADD = \
	$(GRAPPLE_LIBS) $(LUA_LIBS) $(SQLITE_LIBS) $(BULLET_LIBS) $(GLEW_LIBS) \
	$(GL_LIBS) $(OPENAL_LIBS) $(VORBIS_LIBS) $(SQUISH_LIBS) $(ZLIB_LIBS) \
	$(LIPS_LIBS) $(LIBPNG_LIBS) $(PTHREAD_LIBS)
liblipsofsunaclient_la_SOURCES = \
	$(aisrc) $(algsrc) $(arcsrc) $(bndsrc) $(calsrc) $(cfgsrc) $(clisrc) \
	$(fntsrc) $(gensrc) $(engsrc) $(imgsrc) $(matsrc) $(mdlsrc) $(netsrc) \
	$(physrc) $(parsrc) $(pthsrc) $(relsrc) $(rndsrc) $(scrsrc) $(strsrc) \
	$(sndsrc) $(srvsrc) $(syssrc) $(thrsrc) $(vidsrc) $(voxsrc) $(wdgsrc)

liblipsofsunagenerator_la_LDFLAGS = -no-undefined
liblipsofsunagenerator_la_LIBADD = $(SQLITE_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
liblipsofsunagenerator_la_SOURCES = \
	$(algsrc) $(arcsrc) $(calsrc) $(cfgsrc) $(gensrc) $(matsrc) $(mdlsrc) \
	$(pthsrc) $(strsrc) $(syssrc) $(thrsrc) $(voxsrc)

liblipsofsunaimport_la_LDFLAGS = -no-undefined
liblipsofsunaimport_la_LIBADD = $(SQUISH_LIBS) $(SQLITE_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
liblipsofsunaimport_la_SOURCES = \
	$(algsrc) $(arcsrc) $(cfgsrc) $(matsrc) $(mdlsrc) $(pthsrc) $(relsrc) \
	$(strsrc) $(syssrc) $(thrsrc) \
	image/image.c \
	image/image.h \
	image/image-compress.cpp \
	image/image-compress.h \
	image/image-dds.h

liblipsofsunaserver_la_LDFLAGS = -no-undefined
liblipsofsunaserver_la_LIBADD = \
	$(LUA_LIBS) $(GRAPPLE_LIBS) $(SQLITE_LIBS) $(BULLET_LIBS) \
	$(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
liblipsofsunaserver_la_SOURCES = \
	$(aisrc) $(algsrc) $(arcsrc) $(calsrc) $(cfgsrc) $(engsrc) $(matsrc) \
	$(mdlsrc) $(netsrc) $(physrc) $(pthsrc) $(scrsrc) $(srvsrc) $(strsrc) \
	$(syssrc) $(thrsrc) $(voxsrc)

liblipsofsunaviewer_la_LDFLAGS = -no-undefined
liblipsofsunaviewer_la_LIBADD = $(GLEW_LIBS) $(GL_LIBS) $(SQLITE_LIBS) $(SQUISH_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
liblipsofsunaviewer_la_SOURCES = \
	$(algsrc) $(arcsrc) $(cfgsrc) $(fntsrc) $(imgsrc) $(matsrc) $(mdlsrc) \
	$(parsrc) $(pthsrc) $(relsrc) $(rndsrc) $(strsrc) $(syssrc) $(thrsrc) \
	$(vidsrc) $(viesrc)

##############################################################################

lipsofsuna_CFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_LDADD = $(PTHREAD_LIBS) $(LIPS_LIBS)
lipsofsuna_SOURCES = $(maisrc) $(syssrc)

lipsofsuna_client_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_client_LDADD = liblipsofsunaclient.la $(SDL_LIBS) -lSDL_ttf $(LICLI_LIBS)
lipsofsuna_client_SOURCES = client/client-main.c

lipsofsuna_generator_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_generator_LDADD = liblipsofsunagenerator.la $(SQLITE_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
lipsofsuna_generator_SOURCES = generator/generator-main.c

lipsofsuna_import_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_import_LDADD = liblipsofsunaimport.la $(SQUISH_LIBS) $(SQLITE_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
lipsofsuna_import_SOURCES = reload/reload-main.c

lipsofsuna_server_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_server_LDADD = liblipsofsunaserver.la $(LISRV_LIBS)
lipsofsuna_server_SOURCES = server/server-main.c

lipsofsuna_viewer_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_viewer_LDADD = liblipsofsunaviewer.la $(SDL_LIBS) -lSDL_ttf $(GLEW_LIBS) $(GL_LIBS) $(SQLITE_LIBS) $(SQUISH_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
lipsofsuna_viewer_SOURCES = viewer/viewer-main.c

##############################################################################

install-data-local:
	$(mkinstalldirs) $(DATADIR)
	$(mkinstalldirs) $(SAVEDIR)
