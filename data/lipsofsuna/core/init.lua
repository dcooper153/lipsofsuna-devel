Program:add_path(Mod.path)
Program:add_path(Mod.path .. "materials")
Program:add_path(Mod.path .. "shaders")

Voxel:set_block_size(1, 12)

require "common/crafting"
require "common/feat"
require "common/material"
require "common/marker"
require "common/names"
require "common/serialize"
require "common/voxel"
