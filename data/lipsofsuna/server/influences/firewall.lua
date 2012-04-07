Feateffectspec:extend{
	name = "firewall",
	ranged = function(self, args)
		-- Find an empty ground spot.
		local ctr = Utils:find_empty_ground(args.point)
		if not ctr then return end
		-- Select the creation direction.
		local d = (ctr - args.object.position):abs()
		local dir = (d.x < d.z) and Vector(1) or Vector(0,0,1)
		-- Create the flames.
		for i=-2,2 do
			local w = ctr + dir * i * Voxel.tile_size
			local t = (w * Voxel.tile_scale + Vector(0,0.5,0)):floor()
			if Voxel:get_tile(t) == 0 and Voxel:get_tile(t - Vector(0,1)) ~= 0 then
				local feat = Feat{animation = "area spell", effects = {{"burning", 1}}}
				local spec = Spellspec:find{name = "firewall1"}
				AreaSpell{duration = 15, feat = feat, owner = args.owner,
					position = w, realized = true, spec = spec}
			end
		end
	end}
