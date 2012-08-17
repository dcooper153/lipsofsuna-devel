require "system/class"

Sector = Class()

Sector.get_block_offset_by_block_id = function(self, id)
	local bpl = Voxel.blocks_per_line * 128
	local x = id % bpl
	local y = math.floor(id / bpl) % bpl
	local z = math.floor(id / bpl^2) % bpl
	return x,y,z
end

Sector.get_block_id_by_block_offset = function(self, x, y, z)
	local bpl = Voxel.blocks_per_line * 128
	return x + y * bpl + z * bpl^2
end

Sector.get_center_by_id = function(self, id)
	return self:get_offset_by_id(id):add_xyz(0.5,0.5,0.5):
		multiply(Voxel.tiles_per_line * Voxel.tile_size)
end

Sector.get_id_by_offset = function(self, offset)
	local w = 128
	local s = offset:copy():floor()
	return s.x + s.y * w + s.z * w^2
end

Sector.get_id_by_point = function(self, point)
	return self:get_id_by_offset(point * Voxel.tile_scale * (1 / Voxel.tiles_per_line))
end

Sector.get_id_by_tile = function(self, tile)
	return self:get_id_by_offset(tile * (1 / Voxel.tiles_per_line))
end

--- Gets the ID of the sector.
-- @param self Sector.
-- @param tile Sector offset in tiles.
-- @return Number.
Sector.get_id_by_tile = function(self, tile)
	local s = tile:copy():round():divide(Voxel.tiles_per_line)
	return self:get_id_by_offset(s)
end

Sector.get_offset_by_id = function(self, id)
	local w = 128
	local sx = id % w
	local sy = math.floor(id / w) % w
	local sz = math.floor(id / w / w) % w
	return Vector(sx, sy, sz)
end

Sector.get_offset_by_point = function(self, point)
	return point:copy():multiply(Voxel.tile_scale):divide(Voxel.tiles_per_line):round()
end

Sector.get_offset_by_tile = function(self, tile)
	return tile:copy():divide(Voxel.tiles_per_line):round()
end

Sector.get_tile_by_id = function(self, id)
	return self:get_offset_by_id(id):multiply(Voxel.tiles_per_line)
end

Sector.get_tile_by_offset = function(self, offset)
	return offset:copy():multiply(Voxel.tiles_per_line)
end

Sector.get_tile_by_point = function(self, point)
	return self:get_offset_by_point(point):multiply(Voxel.tiles_per_line)
end
