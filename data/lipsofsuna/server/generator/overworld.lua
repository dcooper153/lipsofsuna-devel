require "server/generator"

Generator.Overworld = Class()
Generator.Overworld.class_name = "Generator.Overworld"

--- Generates an overworld area.
-- @param self Overworld generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Overworld.generate = function(self, pos, size)
	local create_plant_or_item = function(ctr)
		-- FIXME: The Y offset is often incorrect.
		ctr = ctr + Vector (0.5, 0, 0.5)
		Voxel:place_obstacle{point = ctr, category = "small-plant"}
	end
	-- Generate objects.
	local c = Vector()
	for i=1,25 do
		c.x = pos.x + math.random(1, size.x - 2)
		c.y = pos.y + math.random(1, size.y - 2)
		c.z = pos.z + math.random(1, size.z - 2)
		local p = Utils:find_spawn_point(c * Voxel.tile_size)
		if p then
			create_plant_or_item(p * Voxel.tile_scale)
		end
	end
end
