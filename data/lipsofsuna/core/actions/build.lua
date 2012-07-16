-- Build terrain or machines.
-- While the attack animation is played, an attack ray is cast.
-- If a tile collides with the ray, a new tile is attached to it.
Actionspec{name = "build", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_build * 0.02)
		-- Check for a correct weapon.
		if not args.weapon then return end
		if not args.weapon.spec.construct_tile then return end
		-- Check for sufficient materials.
		local m = Material:find{name = args.weapon.spec.construct_tile}
		local need = args.weapon.spec.construct_tile_count or 1
		local have = args.weapon.count
		if not m or need > have then return end
		-- Perform the ray cast.
		local src,dst = args.user:get_attack_ray()
		local r = Physics:cast_ray{src = src, dst = dst}
		if not r or r.object then return end
		-- Find the affected tile.
		local t,p = Utils:find_build_point(r.point, args.user)
		if not t then return end
		-- Build the tile.
		args.weapon:subtract(need)
		Voxel:set_tile(p, m.id)
		Server:world_effect(p * Voxel.tile_size, m.effect_build)
	end)
end}
