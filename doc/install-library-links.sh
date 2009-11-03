#! /bin/sh

set -e

mkdir -p build
cd build
rm -f lipsofsuna
cp ../lipsofsuna/lipsofsuna lipsofsuna
cd ..

mkdir -p build/bin
cd build/bin
rm -f lipsofsuna-*
cp ../../lipsofsuna/.libs/lipsofsuna-client lipsofsuna-client
cp ../../lipsofsuna/.libs/lipsofsuna-generator lipsofsuna-generator
cp ../../lipsofsuna/.libs/lipsofsuna-import lipsofsuna-import
cp ../../lipsofsuna/.libs/lipsofsuna-server lipsofsuna-server
cp ../../lipsofsuna/.libs/lipsofsuna-viewer lipsofsuna-viewer
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
libs=`find ../../lipsofsuna/ -name "lib*.so.[0-9].[0-9].[0-9]"`
for i in $libs; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..

mkdir -p build/lib/extensions
cd build/lib/extensions
rm -f *.so
exts="$(find ../../../lipsofsuna/client/extensions -name lib\*.so) \
      $(find ../../../lipsofsuna/server/extensions -name lib\*.so)"
for i in $exts; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
cd ../..
