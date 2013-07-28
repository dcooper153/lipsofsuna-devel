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
	self.__chunks[id] = data
	Main.terrain:reload_chunk(x, z)
end

--- Loads a memorized chunk.<br/>
--
-- This function is called when a chunk has been created by the client. At
-- the time of being called, the memorized chunk list is up to date so we
-- can just read the data from it and add it to the terrain.
--
-- @param self Terrain synchronizer.
-- @param x Chunk offset in grid units.
-- @param z Chunk offset in grid units.
TerrainSync.load_chunk = function(self, x, z)
	local id = Main.terrain:get_chunk_id_by_xz(x, z)
	local data = self.__chunks[id]
	if data then
		Main.terrain.terrain:set_chunk_data(x, z, data)
	end
end

return TerrainSync
