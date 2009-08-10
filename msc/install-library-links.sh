#! /bin/sh

cd build/lib
rm -Rf *
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

mkdir -p extensions
cd extensions
rm -f *
exts=`find ../../../ext/ -name lib\*.so`
for i in $exts; do
	name=$(echo `basename $i` | sed "s/\.[0-9]\.[0-9]//")
	echo $i "->" $name
	ln -s $i $name;
done
