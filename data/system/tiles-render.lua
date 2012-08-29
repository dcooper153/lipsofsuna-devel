--- Provides access to voxel terrain rendering.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.tiles_render
-- @alias VoxelRender

local Class = require("system/class")

if not Los.program_load_extension("tiles-render") then
	error("loading extension `tiles-render' failed")
end

------------------------------------------------------------------------------

--- Provides access to voxel terrain rendering.
-- @type VoxelRender
local VoxelRender = Class("VoxelRender")

--- Sets the viewer position.
-- @param self VoxelRender class.
-- @param value Vector.
VoxelRender.set_viewer_position = function(self, value)
	Los.voxel_render_set_viewer_position(value.handle)
end

--- Sets the viewer rotation.
-- @param self VoxelRender class.
-- @param value Quaternion.
VoxelRender.set_viewer_rotation = function(self, value)
	Los.voxel_render_set_viewer_rotation(value.handle)
end

return VoxelRender
