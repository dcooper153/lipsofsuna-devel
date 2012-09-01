--- Stick-like terrain system.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.terrain
-- @alias Terrain

local Class = require("system/class")
local Packet = require("system/packet")

if not Los.program_load_extension("terrain") then
	error("loading extension `terrain' failed")
end

------------------------------------------------------------------------------

--- Stick-like terrain system.
-- @type Terrain
Terrain = Class("Terrain")

--- Creates a new terrain.
-- @param clss Terrain class.
-- @param chunk_size Number of grid points per chunk.
-- @param grid_size Grid point spacing in world units.
-- @return Terrain.
Terrain.new = function(clss, chunk_size, grid_size)
	local self = Class.new(clss)
	self.handle = Los.terrain_new(chunk_size, grid_size)
	return self
end

--- Adds a stick to the terrain.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @param mat Terrain material number.
-- @return True on success.
Terrain.add_stick = function(self, x, z, y, h, mat)
	return Los.terrain_add_stick(self.handle, x, z, y, h, mat)
end

--- Clears a column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success.
Terrain.clear_column = function(self, x, z)
	return Los.terrain_clear_column(self.handle, x, z)
end

--- Loads a terrain chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success.
Terrain.load_chunk = function(self, x, z)
	return Los.terrain_load_chunk(self.handle, x, z)
end

--- Unloads a terrain chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True if unloaded, false if was already unloaded.
Terrain.unload_chunk = function(self, x, z)
	return Los.terrain_unload_chunk(self.handle, x, z)
end

--- Writes the contents of the chunk to a packet.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return True on success, false if the chunk is not loaded.
Terrain.get_chunk_data = function(self, x, z, packet)
	return Los.terrain_get_chunk_data(self.handle, x, z, packet.handle)
end

--- Reads the contents of the chunk from a packet.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return True on success, false if the chunk is not loaded.
Terrain.set_chunk_data = function(self, x, z, packet)
	return Los.terrain_set_chunk_data(self.handle, x, z, packet.handle)
end

--- Writes the contents of the column to a table.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param t Table.
-- @return Table, or nil the column is not loaded.
Terrain.get_column = function(self, x, z, t)
	return Los.terrain_get_column(self.handle, x, z, t)
end

--- Reads the contents of the column from a table.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet, or nil for a new one.
-- @return True on success, false if the column is not loaded.
Terrain.set_column = function(self, x, z)
	return Los.terrain_set_column(self.handle, x, z)
end

--- Writes the contents of the column to a packet.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return Table, or nil the column is not loaded.
Terrain.get_column_data = function(self, x, z, packet)
	return Los.terrain_get_column_data(self.handle, x, z, packet.handle)
end

--- Reads the contents of the column from a packet.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param packet Packet.
-- @return True on success, false if the column is not loaded.
Terrain.set_column_data = function(self, x, z)
	return Los.terrain_set_column_data(self.handle, x, z, packet.handle)
end

return Terrain
