Feateffectspec{
	name = "respawn",
	influences =
	{
		["respawn"] = 1
	},
	modifier = function(self, mod, secs)
		-- Wait for the sector to load.
		if not Game.terrain:is_point_loaded(mod.object:get_position()) then
			return true
		end
		-- Find the terrain surface.
		local src = mod.object:get_position()
		local dst = Vector(src.x, 0, src.z)
		local p = Game.terrain.terrain:cast_ray(src, dst)
		-- Try another position if failed.
		if not p then
			mod.object:set_position(src.x + 10 * math.random(), 1000, src.z + 10 * math.random())
			return true
		end
		-- Snap to the terrain surface.
		mod.object:set_position(p)
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("respawn", args.value)
	end}
