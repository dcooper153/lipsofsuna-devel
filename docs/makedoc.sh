#! /bin/sh

blddir=build/docs
version=$1

rm -Rf "$blddir/lipsofsuna-docs-$version"
rm -Rf "$blddir/html"
rm -Rf "$blddir/tmp"
mkdir -p "$blddir/lipsofsuna-docs-$version/lua"
mkdir -p "$blddir/tmp"
doxygen "docs/Doxyfile"
ruby "docs/makedoc.rb" "lipsofsuna" "$blddir/tmp"
luadoc --nofiles -d "$blddir/lipsofsuna-docs-$version/lua" "$blddir/tmp"
rm -Rf "$blddir/tmp"
mv "$blddir/html/api" "$blddir/lipsofsuna-docs-$version/"
rm -Rf "$blddir/html"
cd "$blddir"
tar czf "lipsofsuna-docs-$1.tar.gz" "lipsofsuna-docs-$1"
