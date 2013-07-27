--- Synchronizes terrain with the server.
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

--- Synchronizes terrain with the server.
-- @type TerrainSync
local TerrainSync = Class("TerrainSync")

--- Creates a new terrain synchronizer.
-- @param clss TerrainSync class.
-- @return Terrain synchronizer.
TerrainSync.new = function(clss)
	local self = Class.new(clss)
	self.__chunks = {}
	return self
end

--- Clears all memorized blocks.
-- @param self Terrain synchronizer.
TerrainSync.clear = function(self)
	self.__chunks = {}
end

--- Memorizes the terrain for the given chunk.
-- @param self Terrain synchronizer.
-- @param x Chunk offset in grid units.
-- @param z Chunk offset in grid units.
-- @param data Chunk data.
TerrainSync.add_chunk = function(self, x, z, data)
	local id = Main.terrain:get_chunk_id_by_xz(x, z)
	Sector.__chunks[id] = data
	--Voxel:set_block(x, y, z, data)
end

--- Loads a memorized chunk.
-- @param self Terrain synchronizer.
-- @param id Sector ID.
TerrainSync.load_chunk = function(self, x, z)
	if Main.game.mode ~= "join" then return end
--[[
	for x,y,z in Voxel:get_blocks_by_sector_id(id) do
		local block = Sector:get_block_id_by_block_offset(x, y, z)
		local packet = self.blocks_by_id[block]
		if packet then
			packet:read("uint32", "uint32", "uint32")
			Voxel:set_block(x, y, z, packet)
		end
	end--]]
end

return TerrainSync
