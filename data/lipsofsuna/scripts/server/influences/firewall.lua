-- Firewall.
Influencespec{name = "firewall", func = function(feat, info, args, value)
	local p = Utils:find_empty_ground(args.point)
	if not p then return end
	local makewall = function(ctr, dir)
		for i=-2,2 do
			local w = ctr + dir * i * Voxel.tile_size
			local t = (w * Voxel.tile_scale + Vector(0,0.5,0)):floor()
			if Voxel:get_tile(t) == 0 and Voxel:get_tile(t - Vector(0,1)) ~= 0 then
				Spell{effect = "firewall", position = w, power =value}
			end
		end
	end
	local d = (p - args.attacker.position):abs()
	makewall(p, (d.x < d.z) and Vector(1) or Vector(0,0,1))
end}
