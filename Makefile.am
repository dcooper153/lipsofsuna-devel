SUBDIRS = docs .
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(LICLI_CFLAGS)
AM_CXXFLAGS = $(CXXFLAGS) $(LICLI_CFLAGS)
EXTRA_DIST = \
	COPYING.LESSER \
	lipsofsuna/math/unittest.c \
	reload/blender-export.py

if MAYBE_SERVER
SUBDIRS += lipsofsuna/extension/common
endif
if MAYBE_CLIENT
SUBDIRS += lipsofsuna/extension/client
endif
if MAYBE_SERVER
SUBDIRS += lipsofsuna/extension/server
endif

editordir = $(TOOLDIR)
editor_DATA = lipsofsuna/reload/blender-export.py

prgdir = $(PROGDIR)
prg_PROGRAMS = lipsofsuna-launch
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

losdir = $(includedir)/lipsofsuna
los_HEADERS = \
	lipsofsuna/ai.h \
	lipsofsuna/algorithm.h \
	lipsofsuna/archive.h \
	lipsofsuna/binding.h \
	lipsofsuna/callback.h \
	lipsofsuna/client.h \
	lipsofsuna/engine.h \
	lipsofsuna/font.h \
	lipsofsuna/generator.h \
	lipsofsuna/image.h \
	lipsofsuna/math.h \
	lipsofsuna/model.h \
	lipsofsuna/network.h \
	lipsofsuna/particle.h \
	lipsofsuna/physics.h \
	lipsofsuna/paths.h \
	lipsofsuna/reload.h \
	lipsofsuna/render.h \
	lipsofsuna/script.h \
	lipsofsuna/sound.h \
	lipsofsuna/server.h \
	lipsofsuna/string.h \
	lipsofsuna/system.h \
	lipsofsuna/thread.h \
	lipsofsuna/video.h \
	lipsofsuna/voxel.h \
	lipsofsuna/widget.h

aidir = $(includedir)/lipsofsuna/ai
ai_HEADERS = \
	lipsofsuna/ai/ai-manager.h \
	lipsofsuna/ai/ai-path.h \
	lipsofsuna/ai/ai-sector.h \
	lipsofsuna/ai/ai-types.h \
	lipsofsuna/ai/ai-waypoint.h
aisrc = \
	lipsofsuna/ai/ai-manager.c \
	lipsofsuna/ai/ai-path.c \
	lipsofsuna/ai/ai-sector.c

algdir = $(includedir)/lipsofsuna/algorithm
alg_HEADERS = \
	lipsofsuna/algorithm/algorithm-array.h \
	lipsofsuna/algorithm/algorithm-astar.h \
	lipsofsuna/algorithm/algorithm-bst.h \
	lipsofsuna/algorithm/algorithm-camera.h \
	lipsofsuna/algorithm/algorithm-list.h \
	lipsofsuna/algorithm/algorithm-memdic.h \
	lipsofsuna/algorithm/algorithm-octree.h \
	lipsofsuna/algorithm/algorithm-priority-queue.h \
	lipsofsuna/algorithm/algorithm-ptrdic.h \
	lipsofsuna/algorithm/algorithm-range.h \
	lipsofsuna/algorithm/algorithm-range-iter.h \
	lipsofsuna/algorithm/algorithm-sectors.h \
	lipsofsuna/algorithm/algorithm-sectors-iter.h \
	lipsofsuna/algorithm/algorithm-strdic.h \
	lipsofsuna/algorithm/algorithm-u32dic.h
algsrc = \
	lipsofsuna/algorithm/algorithm-astar.c \
	lipsofsuna/algorithm/algorithm-bst.c \
	lipsofsuna/algorithm/algorithm-camera.c \
	lipsofsuna/algorithm/algorithm-octree.c \
	lipsofsuna/algorithm/algorithm-sectors.c

arcdir = $(includedir)/lipsofsuna/archive
arc_HEADERS = \
	lipsofsuna/archive/archive-reader.h \
	lipsofsuna/archive/archive-sql.h \
	lipsofsuna/archive/archive-tar.h \
	lipsofsuna/archive/archive-writer.h
arcsrc = \
	lipsofsuna/archive/archive-reader.c \
	lipsofsuna/archive/archive-sql.c \
	lipsofsuna/archive/archive-tar.c \
	lipsofsuna/archive/archive-writer.c

bnddir = $(includedir)/lipsofsuna/binding
bnd_HEADERS = \
	lipsofsuna/binding/binding.h \
	lipsofsuna/binding/binding-action.h \
	lipsofsuna/binding/binding-manager.h \
	lipsofsuna/binding/binding-types.h
bndsrc = \
	lipsofsuna/binding/binding.c \
	lipsofsuna/binding/binding-action.c \
	lipsofsuna/binding/binding-manager.c

caldir = $(includedir)/lipsofsuna/callback
cal_HEADERS = \
	lipsofsuna/callback/cal-callbacks.h \
	lipsofsuna/callback/cal-handle.h \
	lipsofsuna/callback/cal-marshal.h
calsrc = \
	lipsofsuna/callback/cal-callbacks.c \
	lipsofsuna/callback/cal-handle.c \
	lipsofsuna/callback/cal-marshal.c

clidir = $(includedir)/lipsofsuna/client
cli_HEADERS = \
	lipsofsuna/client/client.h \
	lipsofsuna/client/client-callbacks.h \
	lipsofsuna/client/client-extension.h \
	lipsofsuna/client/client-network.h \
	lipsofsuna/client/client-object.h \
	lipsofsuna/client/client-script.h \
	lipsofsuna/client/client-types.h \
	lipsofsuna/client/client-window.h \
	lipsofsuna/client/widget-login.h
clisrc = \
	lipsofsuna/client/client.c \
	lipsofsuna/client/client-callbacks.c \
	lipsofsuna/client/client-command.c \
	lipsofsuna/client/client-network.c \
	lipsofsuna/client/client-object.c \
	lipsofsuna/client/client-render.c \
	lipsofsuna/client/client-script-action.c \
	lipsofsuna/client/client-script-binding.c \
	lipsofsuna/client/client-script-client.c \
	lipsofsuna/client/client-script-group.c \
	lipsofsuna/client/client-script-extension.c \
	lipsofsuna/client/client-script-light.c \
	lipsofsuna/client/client-script-object.c \
	lipsofsuna/client/client-script-player.c \
	lipsofsuna/client/client-script-scene.c \
	lipsofsuna/client/client-script-widget.c \
	lipsofsuna/client/client-script-window.c \
	lipsofsuna/client/client-window.c \
	lipsofsuna/client/widget-login.c

engdir = $(includedir)/lipsofsuna/engine
eng_HEADERS = \
	lipsofsuna/engine/engine.h \
	lipsofsuna/engine/engine-constraint.h \
	lipsofsuna/engine/engine-iterator.h \
	lipsofsuna/engine/engine-model.h \
	lipsofsuna/engine/engine-object.h \
	lipsofsuna/engine/engine-resources.h \
	lipsofsuna/engine/engine-sector.h \
	lipsofsuna/engine/engine-selection.h \
	lipsofsuna/engine/engine-types.h
engsrc = \
	lipsofsuna/engine/engine.c \
	lipsofsuna/engine/engine-constraint.c \
	lipsofsuna/engine/engine-model.c \
	lipsofsuna/engine/engine-object.c \
	lipsofsuna/engine/engine-resources.c \
	lipsofsuna/engine/engine-sector.c \
	lipsofsuna/engine/engine-selection.c

fntdir = $(includedir)/lipsofsuna/font
fnt_HEADERS = \
	lipsofsuna/font/font.h \
	lipsofsuna/font/font-layout.h
fntsrc = \
	lipsofsuna/font/font.c \
	lipsofsuna/font/font-layout.c

gendir = $(includedir)/lipsofsuna/generator
gen_HEADERS = \
	lipsofsuna/generator/generator.h \
	lipsofsuna/generator/generator-brush.h \
	lipsofsuna/generator/generator-rule.h \
	lipsofsuna/generator/generator-types.h
gensrc = \
	lipsofsuna/generator/generator.c \
	lipsofsuna/generator/generator-brush.c \
	lipsofsuna/generator/generator-rule.c

imgdir = $(includedir)/lipsofsuna/image
img_HEADERS = \
	lipsofsuna/image/image.h \
	lipsofsuna/image/image-compress.h \
	lipsofsuna/image/image-dds.h \
	lipsofsuna/image/image-texture.h
imgsrc = \
	lipsofsuna/image/image.c \
	lipsofsuna/image/image-compress.cpp \
	lipsofsuna/image/image-texture.c

maidir = $(includedir)/lipsofsuna/main
mai_HEADERS = \
	lipsofsuna/main/main-extension.h \
	lipsofsuna/main/main-program.h
maisrc = \
	lipsofsuna/main/main-program.c

matdir = $(includedir)/lipsofsuna/math
mat_HEADERS = \
	lipsofsuna/math/math-aabb.h \
	lipsofsuna/math/math-convert.h \
	lipsofsuna/math/math-frustum.h  \
	lipsofsuna/math/math-generic.h \
	lipsofsuna/math/math-geometry.h \
	lipsofsuna/math/math-halton.h \
	lipsofsuna/math/math-intersect.h \
	lipsofsuna/math/math-matrix.h \
	lipsofsuna/math/math-plane.h \
	lipsofsuna/math/math-polygon.h \
	lipsofsuna/math/math-quadratic.h \
	lipsofsuna/math/math-quaternion.h \
	lipsofsuna/math/math-rectangle.h \
	lipsofsuna/math/math-transform.h \
	lipsofsuna/math/math-triangle.h \
	lipsofsuna/math/math-types.h \
	lipsofsuna/math/math-vector.h \
	lipsofsuna/math/math-vertex.h
matsrc = \
	lipsofsuna/math/math.c

mdldir = $(includedir)/lipsofsuna/model
mdl_HEADERS = \
	lipsofsuna/model/model.h \
	lipsofsuna/model/model-animation.h \
	lipsofsuna/model/model-bone.h \
	lipsofsuna/model/model-faces.h \
	lipsofsuna/model/model-hair.h \
	lipsofsuna/model/model-light.h \
	lipsofsuna/model/model-material.h \
	lipsofsuna/model/model-node.h \
	lipsofsuna/model/model-pose.h \
	lipsofsuna/model/model-shape.h \
	lipsofsuna/model/model-texture.h \
	lipsofsuna/model/model-types.h \
	lipsofsuna/model/model-vertex.h
mdlsrc = \
	lipsofsuna/model/model.c \
	lipsofsuna/model/model-animation.c \
	lipsofsuna/model/model-bone.c \
	lipsofsuna/model/model-faces.c \
	lipsofsuna/model/model-hair.c \
	lipsofsuna/model/model-light.c \
	lipsofsuna/model/model-material.c \
	lipsofsuna/model/model-node.c \
	lipsofsuna/model/model-pose.c \
	lipsofsuna/model/model-shape.c \
	lipsofsuna/model/model-texture.c

netdir = $(includedir)/lipsofsuna/network
net_HEADERS = \
	lipsofsuna/network/network-extension.h \
	lipsofsuna/network/network-protocol.h
netsrc = \
	lipsofsuna/network/network.c

pardir = $(includedir)/lipsofsuna/particle
par_HEADERS = \
	lipsofsuna/particle/particle-line.h \
	lipsofsuna/particle/particle-manager.h \
	lipsofsuna/particle/particle-point.h
parsrc = \
	lipsofsuna/particle/particle-line.c \
	lipsofsuna/particle/particle-manager.c \
	lipsofsuna/particle/particle-point.c

phydir = $(includedir)/lipsofsuna/physics
phy_HEADERS = \
	lipsofsuna/physics/physics.h \
	lipsofsuna/physics/physics-constraint.h \
	lipsofsuna/physics/physics-object.h \
	lipsofsuna/physics/physics-shape.h \
	lipsofsuna/physics/physics-types.h
physrc = \
	lipsofsuna/physics/physics.cpp \
	lipsofsuna/physics/physics-constraint.cpp \
	lipsofsuna/physics/physics-control.cpp \
	lipsofsuna/physics/physics-object.cpp \
	lipsofsuna/physics/physics-private.cpp \
	lipsofsuna/physics/physics-private.h \
	lipsofsuna/physics/physics-shape.cpp

pthdir = $(includedir)/lipsofsuna/path
pth_HEADERS = \
	lipsofsuna/paths/paths.h
pthsrc = \
	lipsofsuna/paths/paths.c

reldir = $(includedir)/lipsofsuna/reload
rel_HEADERS = \
	lipsofsuna/reload/reload.h
relsrc = \
	lipsofsuna/reload/reload.c \
	lipsofsuna/reload/reload-blender.c \
	lipsofsuna/reload/reload-gimp.c \
	lipsofsuna/reload/reload-image.c

rnddir = $(includedir)/lipsofsuna/render
rnd_HEADERS = \
	lipsofsuna/render/render.h \
	lipsofsuna/render/render-buffer.h \
	lipsofsuna/render/render-context.h \
	lipsofsuna/render/render-deferred.h \
	lipsofsuna/render/render-draw.h \
	lipsofsuna/render/render-group.h \
	lipsofsuna/render/render-image.h \
	lipsofsuna/render/render-light.h \
	lipsofsuna/render/render-lighting.h \
	lipsofsuna/render/render-material.h \
	lipsofsuna/render/render-object.h \
	lipsofsuna/render/render-scene.h \
	lipsofsuna/render/render-shader.h \
	lipsofsuna/render/render-texture.h \
	lipsofsuna/render/render-types.h \
	lipsofsuna/render/render-uniform.h
rndsrc = \
	lipsofsuna/render/render.c \
	lipsofsuna/render/render-buffer.c \
	lipsofsuna/render/render-context.c \
	lipsofsuna/render/render-deferred.c \
	lipsofsuna/render/render-draw.c \
	lipsofsuna/render/render-group.c \
	lipsofsuna/render/render-image.c \
	lipsofsuna/render/render-light.c \
	lipsofsuna/render/render-lighting.c \
	lipsofsuna/render/render-material.c \
	lipsofsuna/render/render-model.c \
	lipsofsuna/render/render-model.h \
	lipsofsuna/render/render-object.c \
	lipsofsuna/render/render-scene.c \
	lipsofsuna/render/render-shader.c \
	lipsofsuna/render/render-texture.c

scrdir = $(includedir)/lipsofsuna/script
scr_HEADERS = \
	lipsofsuna/script/script.h \
	lipsofsuna/script/script-args.h \
	lipsofsuna/script/script-class.h \
	lipsofsuna/script/script-data.h \
	lipsofsuna/script/script-event.h \
	lipsofsuna/script/script-library.h \
	lipsofsuna/script/script-packet.h \
	lipsofsuna/script/script-types.h \
	lipsofsuna/script/script-util.h
scrsrc = \
	lipsofsuna/script/script.c \
	lipsofsuna/script/script-args.c \
	lipsofsuna/script/script-class.c \
	lipsofsuna/script/script-data.c \
	lipsofsuna/script/script-event.c \
	lipsofsuna/script/script-library.c \
	lipsofsuna/script/script-object.c \
	lipsofsuna/script/script-packet.c \
	lipsofsuna/script/script-path.c \
	lipsofsuna/script/script-private.h \
	lipsofsuna/script/script-quaternion.c \
	lipsofsuna/script/script-util.c \
	lipsofsuna/script/script-vector.c

snddir = $(includedir)/lipsofsuna/sound
snd_HEADERS = \
	lipsofsuna/sound/sound-manager.h \
	lipsofsuna/sound/sound-sample.h \
	lipsofsuna/sound/sound-source.h \
	lipsofsuna/sound/sound-system.h
sndsrc = \
	lipsofsuna/sound/sound-manager.c \
	lipsofsuna/sound/sound-sample.c \
	lipsofsuna/sound/sound-source.c \
	lipsofsuna/sound/sound-system.c

srvdir = $(includedir)/lipsofsuna/server
srv_HEADERS = \
	lipsofsuna/server/server.h \
	lipsofsuna/server/server-callbacks.h \
	lipsofsuna/server/server-client.h \
	lipsofsuna/server/server-network.h \
	lipsofsuna/server/server-object.h \
	lipsofsuna/server/server-observer.h \
	lipsofsuna/server/server-script.h \
	lipsofsuna/server/server-types.h
srvsrc = \
	lipsofsuna/server/server.c \
	lipsofsuna/server/server-callbacks.c \
	lipsofsuna/server/server-client.c \
	lipsofsuna/server/server-network.c \
	lipsofsuna/server/server-object.c \
	lipsofsuna/server/server-script-effect.c \
	lipsofsuna/server/server-script-extension.c \
	lipsofsuna/server/server-script-object.c \
	lipsofsuna/server/server-script-server.c

strdir = $(includedir)/lipsofsuna/string
str_HEADERS = \
	lipsofsuna/string/string-generic.h
strsrc = \
	lipsofsuna/string/string-generic.c

sysdir = $(includedir)/lipsofsuna/system
sys_HEADERS = \
	lipsofsuna/system/system.h \
	lipsofsuna/system/system-compiler.h \
	lipsofsuna/system/system-directory.h \
	lipsofsuna/system/system-endian.h \
	lipsofsuna/system/system-error.h \
	lipsofsuna/system/system-file.h \
	lipsofsuna/system/system-execute.h \
	lipsofsuna/system/system-memory.h \
	lipsofsuna/system/system-misc.h \
	lipsofsuna/system/system-module.h \
	lipsofsuna/system/system-notify.h \
	lipsofsuna/system/system-path.h \
	lipsofsuna/system/system-relative.h \
	lipsofsuna/system/system-types.h \
	lipsofsuna/system/system-user.h
syssrc = \
	lipsofsuna/system/system.c \
	lipsofsuna/system/system-directory.c \
	lipsofsuna/system/system-error.c \
	lipsofsuna/system/system-file.c \
	lipsofsuna/system/system-execute.c \
	lipsofsuna/system/system-memory.c \
	lipsofsuna/system/system-misc.c \
	lipsofsuna/system/system-module.c \
	lipsofsuna/system/system-notify.c \
	lipsofsuna/system/system-path.c \
	lipsofsuna/system/system-relative.c \
	lipsofsuna/system/system-types.c \
	lipsofsuna/system/system-user.c

thrdir = $(includedir)/lipsofsuna/thread
thr_HEADERS = \
	lipsofsuna/thread/thread.h \
	lipsofsuna/thread/thread-async-call.h
thrsrc = \
	lipsofsuna/thread/thread.c \
	lipsofsuna/thread/thread-async-call.c

viddir = $(includedir)/lipsofsuna/video
vid_HEADERS = \
	lipsofsuna/video/video-calls.h \
	lipsofsuna/video/video-features.h \
	lipsofsuna/video/video-opengl.h
vidsrc = \
	lipsofsuna/video/video-calls.c \
	lipsofsuna/video/video-features.c

viedir = $(includedir)/lipsofsuna/viewer
vie_HEADERS = \
	lipsofsuna/viewer/viewer.h
viesrc = \
	lipsofsuna/viewer/viewer.c

voxdir = $(includedir)/lipsofsuna/voxel
vox_HEADERS = \
	lipsofsuna/voxel/voxel.h \
	lipsofsuna/voxel/voxel-block.h \
	lipsofsuna/voxel/voxel-iterator.h \
	lipsofsuna/voxel/voxel-manager.h \
	lipsofsuna/voxel/voxel-material.h \
	lipsofsuna/voxel/voxel-sector.h \
	lipsofsuna/voxel/voxel-types.h
voxsrc = \
	lipsofsuna/voxel/voxel.c \
	lipsofsuna/voxel/voxel-block.c \
	lipsofsuna/voxel/voxel-manager.c \
	lipsofsuna/voxel/voxel-material.c \
	lipsofsuna/voxel/voxel-private.h \
	lipsofsuna/voxel/voxel-sector.c

wdgdir = $(includedir)/lipsofsuna/widget
wdg_HEADERS = \
	lipsofsuna/widget/widget.h \
	lipsofsuna/widget/widget-busy.h \
	lipsofsuna/widget/widget-button.h \
	lipsofsuna/widget/widget-check.h \
	lipsofsuna/widget/widget-class.h \
	lipsofsuna/widget/widget-container.h \
	lipsofsuna/widget/widget-entry.h \
	lipsofsuna/widget/widget-event.h \
	lipsofsuna/widget/widget-group.h \
	lipsofsuna/widget/widget-image.h \
	lipsofsuna/widget/widget-label.h \
	lipsofsuna/widget/widget-manager.h \
	lipsofsuna/widget/widget-menu.h \
	lipsofsuna/widget/widget-progress.h \
	lipsofsuna/widget/widget-render.h \
	lipsofsuna/widget/widget-scroll.h \
	lipsofsuna/widget/widget-spin.h \
	lipsofsuna/widget/widget-style.h \
	lipsofsuna/widget/widget-tabs.h \
	lipsofsuna/widget/widget-tree.h \
	lipsofsuna/widget/widget-types.h \
	lipsofsuna/widget/widget-view.h \
	lipsofsuna/widget/widget-window.h
wdgsrc = \
	lipsofsuna/widget/widget.c \
	lipsofsuna/widget/widget-busy.c \
	lipsofsuna/widget/widget-button.c \
	lipsofsuna/widget/widget-check.c \
	lipsofsuna/widget/widget-container.c \
	lipsofsuna/widget/widget-entry.c \
	lipsofsuna/widget/widget-group.c \
	lipsofsuna/widget/widget-image.c \
	lipsofsuna/widget/widget-label.c \
	lipsofsuna/widget/widget-manager.c \
	lipsofsuna/widget/widget-menu.c \
	lipsofsuna/widget/widget-private.h \
	lipsofsuna/widget/widget-progress.c \
	lipsofsuna/widget/widget-render.c \
	lipsofsuna/widget/widget-scroll.c \
	lipsofsuna/widget/widget-spin.c \
	lipsofsuna/widget/widget-style.c \
	lipsofsuna/widget/widget-tabs.c \
	lipsofsuna/widget/widget-tree.c \
	lipsofsuna/widget/widget-view.c \
	lipsofsuna/widget/widget-window.c

##############################################################################

liblipsofsunaclient_la_LDFLAGS = -no-undefined
liblipsofsunaclient_la_LIBADD = \
	$(GRAPPLE_LIBS) $(LUA_LIBS) $(SQLITE_LIBS) $(BULLET_LIBS) $(GLEW_LIBS) \
	$(GL_LIBS) $(OPENAL_LIBS) $(VORBIS_LIBS) $(SQUISH_LIBS) $(ZLIB_LIBS) \
	$(LIPS_LIBS) $(LIBPNG_LIBS) $(PTHREAD_LIBS)
liblipsofsunaclient_la_SOURCES = \
	$(aisrc) $(algsrc) $(arcsrc) $(bndsrc) $(calsrc) $(clisrc) $(fntsrc) \
	$(gensrc) $(engsrc) $(imgsrc) $(maisrc) $(matsrc) $(mdlsrc) $(netsrc) \
	$(physrc) $(parsrc) $(pthsrc) $(relsrc) $(rndsrc) $(scrsrc) $(strsrc) \
	$(sndsrc) $(srvsrc) $(syssrc) $(thrsrc) $(vidsrc) $(voxsrc) $(wdgsrc)

liblipsofsunagenerator_la_LDFLAGS = -no-undefined
liblipsofsunagenerator_la_LIBADD = $(SQLITE_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
liblipsofsunagenerator_la_SOURCES = \
	$(algsrc) $(arcsrc) $(calsrc) $(gensrc) $(matsrc) $(mdlsrc) \
	$(pthsrc) $(strsrc) $(syssrc) $(thrsrc) $(voxsrc)

liblipsofsunaimport_la_LDFLAGS = -no-undefined
liblipsofsunaimport_la_LIBADD = $(SQUISH_LIBS) $(SQLITE_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
liblipsofsunaimport_la_SOURCES = \
	$(algsrc) $(arcsrc) $(matsrc) $(mdlsrc) $(pthsrc) $(relsrc) \
	$(strsrc) $(syssrc) $(thrsrc) \
	lipsofsuna/image/image.c \
	lipsofsuna/image/image.h \
	lipsofsuna/image/image-compress.cpp \
	lipsofsuna/image/image-compress.h \
	lipsofsuna/image/image-dds.h

liblipsofsunaserver_la_LDFLAGS = -no-undefined
liblipsofsunaserver_la_LIBADD = \
	$(LUA_LIBS) $(GRAPPLE_LIBS) $(SQLITE_LIBS) $(BULLET_LIBS) \
	$(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
liblipsofsunaserver_la_SOURCES = \
	$(aisrc) $(algsrc) $(arcsrc) $(calsrc) $(engsrc) $(maisrc) $(matsrc) \
	$(mdlsrc) $(netsrc) $(physrc) $(pthsrc) $(scrsrc) $(srvsrc) $(strsrc) \
	$(syssrc) $(thrsrc) $(voxsrc)

liblipsofsunaviewer_la_LDFLAGS = -no-undefined
liblipsofsunaviewer_la_LIBADD = $(GLEW_LIBS) $(GL_LIBS) $(SQLITE_LIBS) $(SQUISH_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
liblipsofsunaviewer_la_SOURCES = \
	$(algsrc) $(arcsrc) $(fntsrc) $(imgsrc) $(matsrc) $(mdlsrc) \
	$(parsrc) $(pthsrc) $(relsrc) $(rndsrc) $(strsrc) $(syssrc) $(thrsrc) \
	$(vidsrc) $(viesrc)

##############################################################################

lipsofsuna_launch_CFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_launch_LDADD = $(PTHREAD_LIBS) $(LIPS_LIBS)
lipsofsuna_launch_SOURCES = lipsofsuna/main/main.c $(syssrc)

lipsofsuna_client_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_client_LDADD = liblipsofsunaclient.la $(SDL_LIBS) -lSDL_ttf $(LICLI_LIBS)
lipsofsuna_client_SOURCES = lipsofsuna/client/client-main.c

lipsofsuna_generator_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_generator_LDADD = liblipsofsunagenerator.la $(SQLITE_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
lipsofsuna_generator_SOURCES = lipsofsuna/generator/generator-main.c

lipsofsuna_import_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_import_LDADD = liblipsofsunaimport.la $(SQUISH_LIBS) $(SQLITE_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(PTHREAD_LIBS)
lipsofsuna_import_SOURCES = lipsofsuna/reload/reload-main.c

lipsofsuna_server_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_server_LDADD = liblipsofsunaserver.la $(LISRV_LIBS)
lipsofsuna_server_SOURCES = lipsofsuna/server/server-main.c

lipsofsuna_viewer_LDFLAGS = $(PTHREAD_CFLAGS) $(LIPS_CFLAGS)
lipsofsuna_viewer_LDADD = liblipsofsunaviewer.la $(SDL_LIBS) -lSDL_ttf $(GLEW_LIBS) $(GL_LIBS) $(SQLITE_LIBS) $(SQUISH_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIPS_LIBS) $(PTHREAD_LIBS)
lipsofsuna_viewer_SOURCES = lipsofsuna/viewer/viewer-main.c

##############################################################################

install-data-local:
	$(mkinstalldirs) $(DATADIR)
	$(mkinstalldirs) $(SAVEDIR)
