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
	self.chunk_size = chunk_size
	self.grid_size = grid_size
	self.handle = Los.terrain_new(chunk_size, grid_size)
	return self
end

--- Adds a sphere to the terrain.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @param mat Terrain material number.
Terrain.add_sphere = function(self, point, radius, mat)
	local materials = t or {}
	for x,z,y,y0,y1,y2,y3 in self:get_sticks_in_sphere(point, radius) do
		self:add_stick_corners(x, z,
			y - y0, y - y1, y - y2, y - y3,
			y + y0, y + y1, y + y2, y + y3, mat)
	end
	return materials
end

--- Adds a sphere to the terrain.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @param mat Terrain material number.
-- @param id Terrain material number for the filter.
Terrain.add_sphere_filter_id = function(self, point, radius, mat, id)
	local materials = t or {}
	for x,z,y,y0,y1,y2,y3 in self:get_sticks_in_sphere(point, radius) do
		self:add_stick_corners_filter_id(x, z,
			y - y0, y - y1, y - y2, y - y3,
			y + y0, y + y1, y + y2, y + y3, mat, id)
	end
	return materials
end

--- Adds a sphere to the terrain.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @param mat Terrain material number.
-- @param mask Terrain material mask for the filter.
Terrain.add_sphere_filter_mask = function(self, point, radius, mat, mask)
	local materials = t or {}
	for x,z,y,y0,y1,y2,y3 in self:get_sticks_in_sphere(point, radius) do
		self:add_stick_corners_filter_mask(x, z,
			y - y0, y - y1, y - y2, y - y3,
			y + y0, y + y1, y + y2, y + y3, mat, mask)
	end
	return materials
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

--- Adds a stick to the terrain.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @param mat Terrain material number.
-- @param id Terrain material number for the filter.
-- @return True on success.
Terrain.add_stick_filter_id = function(self, x, z, y, h, mat, id)
	return Los.terrain_add_stick_filter_id(self.handle, x, z, y, h, mat, id)
end

--- Adds a stick to the terrain.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @param h World unit height.
-- @param mat Terrain material number.
-- @param mask Terrain material mask for the filter.
-- @return True on success.
Terrain.add_stick_filter_mask = function(self, x, z, y, h, mat, mask)
	return Los.terrain_add_stick_filter_mask(self.handle, x, z, y, h, mat, mask)
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
-- @param id Terrain material number for the filter.
-- @return True on success.
Terrain.add_stick_corners_filter_id = function(self, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat, id)
	return Los.terrain_add_stick_corners(self.handle, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat, id)
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
-- @param mask Terrain material mask for the filter.
-- @return True on success.
Terrain.add_stick_corners_filter_mask = function(self, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat, mask)
	return Los.terrain_add_stick_corners(self.handle, x, z, bot00, bot01, bot10, bot11, top00, top01, top10, top11, mat, mask)
end

--- Builds the model for the terrain chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param offset World space offset vector of the chunk, or nil for automatic.
-- @return Model, or nil.
Terrain.build_chunk_model = function(self, x, z, offset)
	local handle = Los.terrain_build_chunk_model(self.handle, x, z, offset and offset.handle)
	if not handle then return end
	return Model:new_from_handle(handle)
end

--- Calculates smooth normals for the (1,1) vertex of the column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success.
Terrain.calculate_smooth_normals = function(self, x, z)
	return Los.terrain_calculate_smooth_normals(self.handle, x, z)
end

--- Calculates smooth normals for vertices in the sphere.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
Terrain.calculate_smooth_normals_in_sphere = function(self, point, radius)
	local r = radius / self.grid_size
	local cx = point.x / self.grid_size
	local cz = point.z / self.grid_size
	local x0 = math.floor(cx - r)
	local x1 = math.floor(cx + r)
	local z0 = math.floor(cz - r)
	local z1 = math.floor(cz + r)
	for z = z0-1,z1+1 do
		for x = x0-1,x1+1 do
			self:calculate_smooth_normals(x, z)
		end
	end
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

--- Clears the model of the chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success, false if the chunk is not loaded.
Terrain.clear_chunk_model = function(self, x, z)
	return Los.terrain_clear_chunk_model(self.handle, x, z)
end

--- Clears a column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return True on success.
Terrain.clear_column = function(self, x, z)
	return Los.terrain_clear_column(self.handle, x, z)
end

--- Counts the materials in the column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y Start of the matched range, or nil for the beginning.
-- @param h Height of the matched range, or nil for infinite.
-- @param t Table to which to add the results, or nil to create a new one.
-- @return Table.
Terrain.count_column_materials = function(self, x, z, y, h, t)
	local t1 = t or {}
	Los.terrain_count_column_materials(self.handle, x, z, y, h, t1)
	return t1
end

--- Counts the materials inside the given sphere.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @param t Table to which to add the results, or nil to create a new one.
-- @return Table of materials.
Terrain.count_materials_in_sphere = function(self, point, radius, t)
	local materials = t or {}
	for x,z,y,y0,y1,y2,y3 in self:get_sticks_in_sphere(point, radius) do
		local avg = (y0 + y1 + y2 + y3) / 4
		self:count_column_materials(x, z, y - avg, avg * 2, materials)
	end
	return materials
end

--- Finds the nearest empty stick in the column.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y Reference Y offset.
-- @param h Minimum stick height. Nil for zero.
-- @return Y offset in world units.
Terrain.find_nearest_empty_stick = function(self, x, z, y, h)
	return Los.terrain_find_nearest_empty_stick(self.handle, x, z, y, h)
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

--- Gets the number of loaded chunks.
-- @param self Terrain.
-- @return Number of loaded chunks.
Terrain.get_chunk_count = function(self)
	return Los.terrain_get_chunk_count(self.handle)
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

--- Gets the time stamp of the chunk.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return Time stamp on success. Nil if the chunk is not loaded.
Terrain.get_chunk_time_stamp = function(self, x, z)
	return Los.terrain_get_chunk_time_stamp(self.handle, x, z)
end

--- Returns an iterator for chunks in the given circle.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @return Iterator.
Terrain.get_chunks_in_circle = function(self, point, radius)
	return coroutine.wrap(function()
		local c = 1 / self.grid_size / self.chunk_size
		local p = Vector(point.x * c, 0, point.z * c)
		local r = radius * c
		local x0 = math.floor(p.x - r)
		local x1 = math.floor(p.x + r)
		local z0 = math.floor(p.z - r)
		local z1 = math.floor(p.z + r)
		for z = z0,z1 do
			for x = x0,x1 do
				local gx = x * self.chunk_size
				local gz = z * self.chunk_size
				local ts = self:get_chunk_time_stamp(gx, gz)
				coroutine.yield(gx, gz, ts)
			end
		end
	end)
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

--- Sets the decoration type of the given material.
-- @param self Terrain.
-- @param index Material index.
-- @param type Stick type.
Terrain.set_material_decoration_type = function(self, index, type)
	return Los.terrain_set_material_decoration_type(self.handle, index, type)
end

--- Sets the stick type of the given material.
-- @param self Terrain.
-- @param index Material index.
-- @param type Stick type.
Terrain.set_material_stick_type = function(self, index, type)
	return Los.terrain_set_material_stick_type(self.handle, index, type)
end

--- Sets the textures of the material.
-- @param self Terrain.
-- @param index Material index.
-- @param top Top texture number.
-- @param bottom Bottom texture number.
-- @param side Side texture number.
-- @param decoration Decoration texture number.
Terrain.set_material_textures = function(self, index, top, bottom, side, decoration)
	return Los.terrain_set_material_textures(self.handle, index, top, bottom, side, decoration)
end

--- Gets the estimated memory consumption of the terrain.
-- @param self Terrain.
-- @return Memory consumption in bytes.
Terrain.get_memory_used = function(self)
	return Los.terrain_get_memory_used(self.handle)
end

--- Finds the nearest chunk whose model is out of date.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @return Grid X and Z coordinates of the chunk, or nil if all models are up to date.
Terrain.get_nearest_chunk_with_outdated_model = function(self, x, z)
	return Los.terrain_get_nearest_chunk_with_outdated_model(self.handle, x, z)
end

--- Gets a stick.
-- @param self Terrain.
-- @param x Grid X coordinate.
-- @param z Grid Z coordinate.
-- @param y World unit Y offset.
-- @return Starting Y offset, height and material.
Terrain.get_stick = function(self, x, z, y)
	return Los.terrain_get_stick(self.handle, x, z, y)
end

--- Returns an iterator for sticks in the given sphere.
-- @param self Terrain.
-- @param point Point in world units.
-- @param radius Radius in world units.
-- @return Iterator.
Terrain.get_sticks_in_sphere = function(self, point, radius)
	return coroutine.wrap(function()
		local t = 0.5 * radius
		local r = radius / self.grid_size
		local cx = point.x / self.grid_size
		local cz = point.z / self.grid_size
		local x0 = math.floor(cx - r)
		local x1 = math.floor(cx + r)
		local z0 = math.floor(cz - r)
		local z1 = math.floor(cz + r)
		local f = function(dx, dz)
			local d = math.sqrt(dx^2 + dz^2) / r
			if d > 1 then return 0 end
			local h = math.cos(d * math.pi / 2) * radius
			if h < t then return 0 end
			return h
		end
		local y = point.y
		for z = z0,z1 do
			for x = x0,x1 do
				local y00 = f(x - cx, z - cz)
				local y10 = f(x - cx + 1, z - cz)
				local y01 = f(x - cx, z - cz + 1)
				local y11 = f(x - cx + 1, z - cz + 1)
				if y00 > 0 or y10 > 0 or y01 > 0 or y11 > 0 then
					coroutine.yield(x, z, y, y00, y10, y01, y11)
				end
			end
		end
	end)
end

return Terrain
