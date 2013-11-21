#!/bin/sh

SDKDIR=$1

cp $SDKDIR/bin/libiconv-2.dll .
cp $SDKDIR/bin/pthreadGC2.dll .
cp $SDKDIR/bin/libgcc_s_dw2-1.dll .
cp $SDKDIR/bin/libstdc++-6.dll .
cp $SDKDIR/msys/1.0/local/bin/FreeImage.dll .
cp $SDKDIR/msys/1.0/local/bin/libBulletCollision.dll .
cp $SDKDIR/msys/1.0/local/bin/libBulletDynamics.dll .
cp $SDKDIR/msys/1.0/local/bin/libLinearMath.dll .
cp $SDKDIR/msys/1.0/local/bin/libcurl-4.dll .
cp $SDKDIR/msys/1.0/local/bin/OgreMain.dll .
cp $SDKDIR/msys/1.0/local/bin/OgreTerrain.dll .
cp $SDKDIR/msys/1.0/local/bin/libOIS.dll .
cp $SDKDIR/msys/1.0/local/bin/libsqlite3-0.dll .
cp $SDKDIR/msys/1.0/local/bin/libvorbisfile-3.dll .
cp $SDKDIR/msys/1.0/local/bin/libvorbisfile-3.dll .
cp $SDKDIR/msys/1.0/local/bin/libboost_thread-mgw45-mt-1_44.dll .
cp $SDKDIR/msys/1.0/local/bin/libvorbis-0.dll .
cp $SDKDIR/msys/1.0/local/bin/libogg-0.dll .
cp $SDKDIR/msys/1.0/local/bin/OgrePaging.dll .
mkdir -p plugins
cp $SDKDIR/msys/1.0/local/bin/RenderSystem_GL.dll plugins
cp $SDKDIR/msys/1.0/local/bin/Plugin_ParticleFX.dll plugins
cp $SDKDIR/msys/1.0/local/bin/Plugin_OctreeSceneManager.dll plugins
