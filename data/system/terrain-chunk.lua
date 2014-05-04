--- Stick-like terrain chunk.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.terrain_chunk
-- @alias TerrainChunk

local Class = require("system/class")

if not Los.program_load_extension("terrain") then
	error("loading extension `terrain' failed")
end

------------------------------------------------------------------------------

--- Stick-like terrain chunk.
-- @type TerrainChunk
local TerrainChunk = Class("TerrainChunk")

--- Creates a new terrain.
-- @param clss TerrainChunk class.
-- @param chunk_size Number of grid points per chunk.
-- @return TerrainChunk.
TerrainChunk.new = function(clss, chunk_size)
	local self = Class.new(clss)
	self.chunk_size = chunk_size
	self.handle = Los.terrain_chunk_new(chunk_size)
	return self
end

--- Adds a box to the terrain chunk.
-- @param self TerrainChunk.
-- @param x1 Minimum grid X coordinate.
-- @param z1 Minimum grid Z coordinate.
-- @param x2 Maximum grid X coordinate.
-- @param z2 Maximum grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @param mat Terrain material number.
-- @return True on success. False otherwise.
TerrainChunk.add_box = function(self, x1, z1, x2, z2, y, h, mat)
	return Los.terrain_chunk_add_box(self.handle, x1, z1, x2, z2, y, h, mat)
end

--- Adds a box to the terrain chunk using corner offsets.
-- @param self TerrainChunk.
-- @param x1 Minimum grid X coordinate.
-- @param z1 Minimum grid Z coordinate.
-- @param x2 Maximum grid X coordinate.
-- @param z2 Maximum grid Z coordinate.
-- @param bot00 World unit Y offset of the bottom vertices.
-- @param bot10 World unit Y offset of the bottom vertices.
-- @param bot01 World unit Y offset of the bottom vertices.
-- @param bot11 World unit Y offset of the bottom vertices.
-- @param top00 World unit Y offset of the top vertices.
-- @param top10 World unit Y offset of the top vertices.
-- @param top01 World unit Y offset of the top vertices.
-- @param top11 World unit Y offset of the top vertices.
-- @param mat Terrain material number.
-- @return True on success. False otherwise.
TerrainChunk.add_box_corners = function(self, x1, z1, x2, z2, bot00, bot10, bot01, bot11, top00, top10, top01, top11, mat)
	return Los.terrain_chunk_add_box_corners(self.handle, x1, z1, x2, z2, bot00, bot10, bot01, bot11, top00, top10, top01, top11, mat)
end

--- Adds a stick to the terrain chunk.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @param mat Terrain material number.
-- @return True on success. False otherwise.
TerrainChunk.add_stick = function(self, x, z, y, h, mat)
	return Los.terrain_chunk_add_stick(self.handle, x, z, y, h, mat)
end

--- Adds a stick to the terrain chunk using corner offsets.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param bot00 World unit Y offset of the bottom vertices.
-- @param bot10 World unit Y offset of the bottom vertices.
-- @param bot01 World unit Y offset of the bottom vertices.
-- @param bot11 World unit Y offset of the bottom vertices.
-- @param top00 World unit Y offset of the top vertices.
-- @param top10 World unit Y offset of the top vertices.
-- @param top01 World unit Y offset of the top vertices.
-- @param top11 World unit Y offset of the top vertices.
-- @param mat Terrain material number.
-- @return True on success. False otherwise.
TerrainChunk.add_stick_corners = function(self, x, z, bot00, bot10, bot01, bot11, top00, top10, top01, top11, mat)
	return Los.terrain_chunk_add_stick_corners(self.handle, x, z, bot00, bot10, bot01, bot11, top00, top10, top01, top11, mat)
end

--- Clears a column.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success. False otherwise.
TerrainChunk.clear_column = function(self, x, z)
	return Los.terrain_chunk_clear_column(self.handle, x, z)
end

--- Writes the contents of the chunk to a packet.
-- @param self TerrainChunk.
-- @param packet Packet.
-- @return True on success. False otherwise.
TerrainChunk.get_chunk_data = function(self, packet)
	return Los.terrain_chunk_get_chunk_data(self.handle, packet.handle)
end

--- Reads the contents of the chunk from a packet.
-- @param self TerrainChunk.
-- @param packet Packet.
-- @return True on success. False otherwise.
TerrainChunk.set_chunk_data = function(self, packet)
	return Los.terrain_chunk_set_chunk_data(self.handle, packet.handle)
end

--- Writes the contents of the column to a table.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param t Table.
-- @return Table on success. Nil otherwise.
TerrainChunk.get_column = function(self, x, z, t)
	return Los.terrain_chunk_get_column(self.handle, x, z, t)
end

--- Reads the contents of the column from a table.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet, or nil for a new one.
-- @return True on success. False otherwise.
TerrainChunk.set_column = function(self, x, z)
	return Los.terrain_chunk_set_column(self.handle, x, z)
end

--- Writes the contents of the column to a packet.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return Table on success. Nil otherwise.
TerrainChunk.get_column_data = function(self, x, z, packet)
	return Los.terrain_chunk_get_column_data(self.handle, x, z, packet.handle)
end

--- Reads the contents of the column from a packet.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return True on success. False otherwise.
TerrainChunk.set_column_data = function(self, x, z)
	return Los.terrain_chunk_set_column_data(self.handle, x, z, packet.handle)
end

--- Gets a stick.
-- @param self TerrainChunk.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @return Starting Y offset, height and material.
TerrainChunk.get_stick = function(self, x, z, y)
	return Los.terrain_chunk_get_stick(self.handle, x, z, y)
end

return TerrainChunk
