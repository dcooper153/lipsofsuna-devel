--- Building subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module building.building
-- @alias Building

local Class = require("system/class")
local Hooks = require("system/hooks")
local TerrainManager = require("core/terrain/terrain-manager")
local Vector = require("system/math/vector")

--- Building subgame.
-- @type Building
Building = Class("Building")

--- Creates the building subgame.
-- @param clss Building class.
-- @return Building.
Building.new = function(clss)
	local self = Class.new(clss)
	-- Initialize the terrain.
	self.terrain = TerrainManager(12, 1, nil, false, true, true)
	self.terrain:set_view_center(Vector(500, 0, 500))
	self.terrain.generate_hooks:register(0, function(self)
		local w = self.manager.chunk_size
		local t = self.manager.terrain
		for z = 0,w-1 do
			for x = 0,w-1 do
				t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, 100, 100, 100, 100, 3)
			end
		end
		for x = self.x-1,self.x+w do
			for z = self.z-1,self.z+w do
				t:calculate_smooth_normals(x, z)
			end
		end
		return Hooks.STOP
	end)
	return self
end

--- Closes the subgame.
-- @param self Building.
Building.close = function(self)
	-- TODO
end

--- Updates the subgame state.
-- @param self Building.
-- @param secs Seconds since the last update.
Building.update = function(self, secs)
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	self.terrain:refresh_chunks_by_point(Vector(500, 0, 500), 20)
end

return Building
