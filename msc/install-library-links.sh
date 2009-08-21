#! /bin/sh

set -e

mkdir -p build
cd build
rm -f lipsofsuna
cp ../src/lipsofsuna lipsofsuna
cd ..

mkdir -p build/bin
cd build/bin
rm -f lipsofsuna-*
cp ../../src/.libs/lipsofsuna-client lipsofsuna-client
cp ../../src/.libs/lipsofsuna-generator lipsofsuna-generator
cp ../../src/.libs/lipsofsuna-import lipsofsuna-import
cp ../../src/.libs/lipsofsuna-server lipsofsuna-server
cp ../../src/.libs/lipsofsuna-viewer lipsofsuna-viewer
cd ../..

mkdir -p build/tool
cd build/tool
rm -f *.py
ln -s ../../data/blender-export.py blender-export.py
cd ../..

mkdir -p build/lib
cd build/lib
rm -f *.so*
libs=`find ../../dep/ -name "lib*.so.[0-9].[0-9].[0-9]"`
for i in $libs; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
libs=`find ../../src/ -name "lib*.so.[0-9].[0-9].[0-9]"`
for i in $libs; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..

mkdir -p build/lib/extensions
cd build/lib/extensions
rm -f *.so
exts=`find ../../../ext/ -name lib\*.so`
for i in $exts; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..
