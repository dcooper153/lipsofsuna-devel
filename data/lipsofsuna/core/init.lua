Program:add_path(Mod.path)
Program:add_path(Mod.path .. "materials")
Program:add_path(Mod.path .. "shaders")

Voxel.blocks_per_line = 1
Voxel.tiles_per_line = 12
require "common/color"
require "common/crafting"
require "common/feat"
require "common/material"
require "common/marker"
require "common/model"
require "common/names"
require "common/serialize"
require "common/sectors"
require "common/voxel"
require "system/lobby"
