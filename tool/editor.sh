#!/bin/sh

echo "Available patterns: "
echo $(ls ./data/lipsofsuna/scripts/content/patterns)

NUM=1

while [ -e ./data/lipsofsuna/scripts/content/patterns/$@mod$NUM.lua ] ; do

  echo " $NUM file exists, incrementing "
  NUM=$(($NUM+1)) 

done

./lipsofsuna lipsofsuna -E $@ > ./data/lipsofsuna/scripts/content/patterns/$@mod$NUM.lua
     
