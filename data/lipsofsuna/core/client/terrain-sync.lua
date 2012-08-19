local Class = require("system/class")
local Sector = require("system/sector")

local TerrainSync = Class()

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
	if Game.mode ~= "join" then return end
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
