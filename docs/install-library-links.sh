#! /bin/sh

set -e

mkdir -p build
cd build
rm -f lipsofsuna
cp ../lipsofsuna-launch lipsofsuna
cd ..

mkdir -p build/bin
cd build/bin
rm -f lipsofsuna-*
cp ../../.libs/lipsofsuna-client lipsofsuna-client
cp ../../.libs/lipsofsuna-generator lipsofsuna-generator
cp ../../.libs/lipsofsuna-import lipsofsuna-import
cp ../../.libs/lipsofsuna-server lipsofsuna-server
cp ../../.libs/lipsofsuna-viewer lipsofsuna-viewer
cd ../..

mkdir -p build/tool
cd build/tool
rm -f *.py
ln -s ../../lipsofsuna/reload/blender-export.py blender-export.py
cd ../..

mkdir -p build/lib
cd build/lib
rm -f *.so*
libs=`find /opt/lipsofsuna -name "lib*.so.[0-9].[0-9].[0-9]"`
for i in $libs; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
libs=`find ../../.libs/ -name "lib*.so.[0-9].[0-9].[0-9]"`
for i in $libs; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..

mkdir -p build/lib/extensions
cd build/lib/extensions
rm -f *.so
exts="$(find ../../../lipsofsuna/extension -name lib\*.so)"
for i in $exts; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..
