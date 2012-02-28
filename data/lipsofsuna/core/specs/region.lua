require(Mod.path .. "spec")

Regionspec = Class(Spec)
Regionspec.type = "region"
Regionspec.dict_id = {}
Regionspec.dict_cat = {}
Regionspec.dict_name = {}

--- Creates a new region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>links: Array of names of connected regions.</li>
--   <li>name: Unique region name.</li>
--   <li>random_resources: False to disable generation of random resources.</li>
--   <li>size: Region size in tiles.</li></ul>
--   <li>spawn_point: Spawn point position in tiles.</li></ul>
-- @return New regionspec.
Regionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.links = self.links or {}
	self.random_resources = (args.random_resources ~= false)
	self.size = self.size or {4,4,4}
	return self
end

--- Finds regions that have a spawn point.
-- @param self Regionspec class.
-- @return Table of regions.
Regionspec.find_spawn_points = function(self)
	local list = {}
	for k,v in pairs(self.dict_id) do
		if v.spawn_point then
			table.insert(list, v)
		end
	end
	return list
end

Regionspec:add_getters{
	spawn_point_world = function(self)
		if not self.spawn_point then return end
		local o = Vector(self.position[1], self.depth[1], self.position[2])
		local p = Vector(self.spawn_point[1], self.spawn_point[2], self.spawn_point[3])
		return (o + p) * Voxel.tile_size
	end}
