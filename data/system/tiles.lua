local Class = require("system/class")
local Packet = require("system/packet")
local Sector = require("system/sector")

if not Los.program_load_extension("tiles") then
	error("loading extension `tiles' failed")
end

------------------------------------------------------------------------------

Voxel = Class("Voxel")

--- Copies a terrain region into a packet.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Tile index vector.</li>
--   <li>sector: Sector index.</li>
--   <li>size: Region size, in tiles.</li></ul>
-- @return Packet writer.
Voxel.copy_region = function(self, args)
	local handle = Los.voxel_copy_region{point = args.point and args.point.handle, sector = args.sector, size = args.size}
	return Packet:new_from_handle(handle)
end

--- Fills a terrain region.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Tile index vector.</li>
--   <li>size: Size vector.</li>
--   <li>tile: Tile data.</ul>
Voxel.fill_region = function(self, args)
	return Los.voxel_fill_region{point = args.point.handle, size = args.size.handle, tile = args.tile}
end

--- Finds all blocks near the given point.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>radius: Radius.</li></ul>
-- @return Table of block indices and modification stamps.
Voxel.find_blocks = function(self, args)
	return Los.voxel_find_blocks{point = args.point.handle, radius = args.radius}
end

--- Finds the tile nearest to the given point.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>match: Tiles to search for. ("all"/"empty"/"full")</li>
--   <li>point: Position vector in world space.</li>
--   <li>radius: Search radius in tiles.</li></ul>
-- @return Tile and voxel index vector, or nil.
Voxel.find_tile = function(self, args)
	local t,p = Los.voxel_find_tile{match = args.match, point = args.point.handle, radius = args.radius}
	if not t then return end
	return t, Vector:new_from_handle(p)
end

--- Intersects a ray with map tiles.
-- @param self Object.
-- @param s Ray start point, in world space.</li>
-- @param e Ray end point, in world space.</li>
-- @return Position vector in world space, tile index vector.
Voxel.intersect_ray = function(self, s, e)
	return Los.voxel_intersect_ray(s.handle, e.handle)
end

--- Pastes a terrain region from a packet to the map.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>packet: Data packet.</li>
--   <li>point: Tile index vector.</li>
--   <li>sector: Sector index.</li></ul>
Voxel.paste_region = function(self, args)
	Los.voxel_paste_region{packet = args.packet.handle, point = args.point and args.point.handle, sector = args.sector}
end

--- Update the voxel terrain state.
-- @param self Voxel class.
-- @param secs Seconds since the last update.
Voxel.update = function(self, secs)
	Los.voxel_update(secs)
end

--- Gets the data of a voxel block.
-- @param self Voxel class.
-- @param x Block offset.
-- @param y Block offset.
-- @param z Block offset.
-- @param packet Packet writer.
Voxel.get_block = function(self, x, y, z, packet)
	Los.voxel_get_block(x, y, z, packet.handle)
end

--- Sets the contents of a block of voxels.
-- @param self Voxel class.
-- @param x Block offset.
-- @param y Block offset.
-- @param z Block offset.
-- @param packet Packet reader.
-- @return True on success.
Voxel.set_block = function(self, x, y, z, packet)
	return Los.voxel_set_block(x, y, z, packet.handle)
end

--- Gets the offsets of the blocks in the given sector.
-- @param self Voxel class.
-- @param id Sector ID.
-- @return Iterator over the offsets.
Voxel.get_blocks_by_sector_id = function(self, id)
	return coroutine.wrap(function()
		local bps = self.blocks_per_line
		local bpl = bps * 128
		local org = Sector:get_offset_by_id(id):multiply(bps)
		for z = 0,self.blocks_per_line-1 do
			for y = 0,self.blocks_per_line-1 do
				for x = 0,self.blocks_per_line-1 do
					coroutine.yield(org.x + x, org.y + y, org.z + z)
				end
			end
		end
	end)
end

--- Sets the block size of voxel terrain.
-- @param self Voxel class.
-- @param blocks Number of blocks along a sector edge.
-- @param tiles Number of tiles along a block edge.
Voxel.set_block_size = function(self, blocks, tiles)
	Los.voxel_set_blocks_per_line(blocks)
	Los.voxel_set_tiles_per_line(tiles)
	-- TODO: Get rid of these shortcuts?
	self.blocks_per_line = blocks
	self.tiles_per_line = tiles
end

--- Gets the fill tile number for empty sectors.
-- @param self Voxel class.
-- @return Number.
Voxel.get_fill_tile = function(self)
	return Los.voxel_get_fill()
end

--- Sets the fill tile number for empty sectors.
-- @param self Voxel class.
-- @param v Number.
Voxel.set_fill_tile = function(self, v)
	Los.voxel_set_fill(v)
end

--- Gets the approximate memory used by voxel terrain, in bytes.
-- @param self Voxel class.
-- @return Memory used, in bytes.
Voxel.get_memory_used = function(self)
	return Los.voxel_get_memory_used()
end

--- Gets the contents of a tile.
-- @param self Voxel class.
-- @param coord Tile offset vector.
-- @return Tile.
Voxel.get_tile = function(self, coord)
	return Los.voxel_get_tile(coord.handle)
end

--- Sets the contents of a tile.
-- @param self Voxel class.
-- @param coord Vector.
-- @param tile Tile number.
Voxel.set_tile = function(self, coord, tile)
	return Los.voxel_set_tile(coord.handle, tile)
end

--- Sets the contents of multiple tiles.
-- @param self Voxel class.
-- @param tile Tile number.
-- @param coords List of vectors.</li></ul>
Voxel.set_tiles = function(self, tile, coords)
	local h = {}
	for k,v in pairs(coords) do h[k] = coords[k].handle end
	return Los.voxel_set_tiles(tile, h)
end

--- Number of blocks along a sector edge.
-- @name Voxel.blocks_per_line
-- @class table

--- Number of tiles along a sector edge.
-- @name Voxel.tiles_per_line
-- @class table

return Voxel
