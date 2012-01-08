#! /bin/bash

if test "x$1" = "x"; then
	echo "Usage: $0 <extname>"
	exit 0
fi

generatefile ()
{
	tmp=$2
	classa="$tmp"
	classb="$(echo $tmp | tr "[:lower:]" "[:upper:]")"
	classc="$(echo ${tmp:0:1} | tr "[:lower:]" "[:upper:]")${tmp:1}"
	cat $1 | sed s/Skeleton/$classc/g | sed s/SKELETON/$classb/g | sed s/skeleton/$classa/g
}

mkdir ../$1
generatefile module.c $1 > ../$1/module.c
generatefile module.h $1 > ../$1/module.h
generatefile script.c $1 > ../$1/script.c
