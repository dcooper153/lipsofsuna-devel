--- Gets the center of the sector in world space coordinates.
-- @param self Sectors.
-- @return Position vector, in world space.
Sectors.get_sector_center = function(self, sector)
	local t = self:get_sector_offset(sector)
	local w = Voxel.tiles_per_line / 2
	return (t + Vector(w,w,w)) * Voxel.tile_size
end

--- Gets the offset of the sector in tiles.
-- @param self Sectors.
-- @param sector Sector index.
-- @return Vector.
Sectors.get_sector_offset = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	return Vector(sx, sy, sz) * Voxel.tiles_per_line
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
	if not Server.initialized then return end
	-- Create fractal terrain for newly found sectors.
	if not terrain then Server.generator:generate_sector(sector) end
	-- Spawn monsters and events.
	Server.events:sector_created(sector, terrain, objects)
end
