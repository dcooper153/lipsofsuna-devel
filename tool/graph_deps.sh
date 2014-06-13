#!/bin/sh
tool/code_search.lua -d | dot -Tpng > deps.png
eog deps.png
