--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.terrain_sync
-- @alias TerrainSync

local Class = require("system/class")
local Sector = require("system/sector")

--- TODO:doc
-- @type TerrainSync
local TerrainSync = Class("TerrainSync")

--- Creates a new terrain synchronizer.
-- @param clss TerrainSync class.
-- @return Terrain synchronizer.
TerrainSync.new = function(clss)
	local self = Class.new(clss)
	self.blocks_by_id = {}
	return self
end

--- Clears all memorized blocks.
-- @param self Terrain synchronizer.
TerrainSync.clear = function(self)
	self.blocks_by_id = {}
end

--- Memorized terrain for the given block.
-- @param self Terrain synchronizer.
-- @param x Block offset.
-- @param y Block offset.
-- @param z Block offset.
-- @param data Block data.
TerrainSync.add_block = function(self, x, y, z, data)
	local block = Sector:get_block_id_by_block_offset(x, y, z)
	self.blocks_by_id[block] = data
	Voxel:set_block(x, y, z, data)
end

--- Loads memorized blocks for the given sector.
-- @param self Terrain synchronizer.
-- @param id Sector ID.
TerrainSync.load_sector = function(self, id)
	if Main.game.mode ~= "join" then return end
	for x,y,z in Voxel:get_blocks_by_sector_id(id) do
		local block = Sector:get_block_id_by_block_offset(x, y, z)
		local packet = self.blocks_by_id[block]
		if packet then
			packet:read("uint32", "uint32", "uint32")
			Voxel:set_block(x, y, z, packet)
		end
	end
end

return TerrainSync


