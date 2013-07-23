--- Physics constants.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.physics_consts
-- @alias PhysicsConsts

local Class = require("system/class")

--- Physics constants.
-- @type PhysicsConsts
local PhysicsConsts = Class("PhysicsConsts")

PhysicsConsts.GROUP_ACTORS = 0x0001
PhysicsConsts.GROUP_ITEMS = 0x0002
PhysicsConsts.GROUP_PLAYERS = 0x0004
PhysicsConsts.GROUP_TERRAIN = 0x0800
PhysicsConsts.GROUP_OBSTACLES = 0x1000
PhysicsConsts.GROUP_STATICS = 0x2000
PhysicsConsts.GROUP_HEIGHTMAP = 0x4000
PhysicsConsts.GROUP_VOXELS = 0x8000
PhysicsConsts.MASK_ALL = 0xFFFF
PhysicsConsts.MASK_CAMERA = 0xFF03
PhysicsConsts.MASK_PICK = 0xFF03
PhysicsConsts.MASK_TERRAIN = 0x00FF

return PhysicsConsts
