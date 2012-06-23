-- Melee attack.
-- While the attack animation is played, the blade is moved along a path.
-- The first object or tile that collides with the blade ray is damaged.
-- The shape of the blade path depends on the controls of the user.
Actionspec{name = "melee", func = function(feat, info, args)
	local paths = {
		--[[Stand]] {Vector(0.1, 0, 0.75), Vector(-0.1, 0, 0.5), Vector(0.05, 0, 0.25), Vector(-0.05, 0, 0)},
		--[[Left]] {Vector(0.5, 0.1, 1), Vector(0.25, 0.05, 0.7), Vector(0, 0, 0.5), Vector(-0.25, 0, 0.7)},
		--[[Right]] {Vector(0.5, 0.1, 0.7), Vector(0.1, 0.05, 0.5), Vector(-0.2, 0, 0.6), Vector(-0.5, 0, 1)},
		--[[Back]] {Vector(0.1, 0.2, 1), Vector(0.05, 0.2, 0.75), Vector(0, 0.1, 0.5), Vector(0, -0.1, 0.25)},
		--[[Forward]] {Vector(0.3, -0.1, 0.8), Vector(0.2, -0.1, 0.55), Vector(0.1, -0.05, 0.3), Vector(0, 0.05, 0.15)}}
	local move
	local path
	local anim = Feattypespec:find{name = feat.animation}
	if anim and anim.directional then
		move = args.user.attack_charge_move
		if move then
			if move == "left" then move = 2
			elseif move == "right" then move = 3
			elseif move == "back" then move = 4
			elseif move == "front" then move = 5
			else move = 1 end
		else
			if args.user.strafing < -0.2 then move = 2
			elseif args.user.strafing > 0.2 then move = 3
			elseif args.user.movement < -0.2 then move = 4
			elseif args.user.movement > 0.2 then move = 5
			else move = 1 end
		end
		path = paths[move]
	else
		path = paths[1]
	end
	Coroutine(function(t)
		local apply = function(r)
			local args = {
				charge = args.charge,
				object = r.object,
				owner = args.user,
				point = r.point,
				tile = r.tile,
				weapon = args.weapon}
			feat:play_effects_impact(args)
			feat:apply_impulse(args)
			feat:apply_digging(args)
			feat:apply_touch(args)
			return true
		end
		local prev
		local mask = Physics.GROUP_ACTORS + Physics.GROUP_ITEMS
		local cast = function(rel)
			-- Get the attack ray.
			local src,dst = args.user:get_attack_ray(rel)
			-- Cast from the previous point.
			local r = prev and Physics:cast_ray{src = prev, dst = dst, collision_mask = mask}
			if r then return apply(r) end
			prev = dst
		end
		-- Cast a curve against actors and items.
		feat:play_effects(args)
		for i = 1,4 do
			Coroutine:sleep(args.user.spec.timing_attack_melee * 0.02 / 4)
			if cast(path[i]) then return end
		end
		-- Cast a straight ray against everything.
		local src,dst = args.user:get_attack_ray()
		local r = Physics:cast_ray{src = src, dst = dst}
		if r then return apply(r) end
	end)
	return move
end}
