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
local Model = require("system/model")
local Packet = require("system/packet")
local Vector = require("system/math/vector")

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

--- Adds a stick to the terrain using corner offsets.
-- @param self Terrain.
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
-- @return True on success.
Terrain.add_stick_corners = function(self, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat)
	return Los.terrain_add_stick_corners(self.handle, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat)
end

--- Builds the model for the terrain chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return Model, or nil.
Terrain.build_chunk_model = function(self, x, z)
	local handle = Los.terrain_build_chunk_model(self.handle, x, z)
	if not handle then return end
	return Model:new_from_handle(handle)
end

--- Casts a ray against the terrain.
-- @param self Terrain.
-- @param src Source point, in world units.
-- @param dst Destination point, in world units.
-- @return On hit: Point vector, normal vector, grid X, grid Z. Nil if no hit.
Terrain.cast_ray = function(self, src, dst)
	local gx,gy,px,py,pz,nx,ny,nz = Los.terrain_cast_ray(self.handle, src.handle, dst.handle)
	if not gx then return end
	return Vector(px,py,pz),Vector(nx,ny,nz),gx,gy
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

--- Smoothens the vertices of the column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @return True on success.
Terrain.smoothen_column = function(self, x, z, y, h)
	return Los.terrain_smoothen_column(self.handle, x, z, y, h)
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
	packet:read()
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

--- Finds the nearest chunk whose model is out of date.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return Grid X and Z coordinates of the chunk, or nil if all models are up to date.
Terrain.get_nearest_chunk_with_outdated_model = function(self, x, z)
	return Los.terrain_get_nearest_chunk_with_outdated_model(self.handle, x, z)
end

return Terrain
