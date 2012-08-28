local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local Physics = require("system/physics")

local choose_move = function(attacker)
	if attacker:get_strafing() < -0.2 then return "left"
	elseif attacker:get_strafing() > 0.2 then return "right"
	elseif attacker:get_movement() < -0.2 then return "back"
	elseif attacker:get_movement() > 0.2 then return "front"
	else return "stand" end
end

local choose_path = function(attacker, move)
	local m = move or choose_move(attacker)
	local paths = {
		["stand"] = {Vector(0.1, 0, 0.75), Vector(-0.1, 0, 0.5), Vector(0.05, 0, 0.25), Vector(-0.05, 0, 0)},
		["left"] = {Vector(0.5, 0.1, 1), Vector(0.25, 0.05, 0.7), Vector(0, 0, 0.5), Vector(-0.25, 0, 0.7)},
		["right"] = {Vector(0.5, 0.1, 0.7), Vector(0.1, 0.05, 0.5), Vector(-0.2, 0, 0.6), Vector(-0.5, 0, 1)},
		["back"] = {Vector(0.1, 0.2, 1), Vector(0.05, 0.2, 0.75), Vector(0, 0.1, 0.5), Vector(0, -0.1, 0.25)},
		["front"] = {Vector(0.3, -0.1, 0.8), Vector(0.2, -0.1, 0.55), Vector(0.1, -0.05, 0.3), Vector(0, 0.05, 0.15)}}
	return paths[move]
end

local perform_attack = function(attacker, move)
	local prev
	local weapon = attacker:get_weapon()
	local path = choose_path(attacker, move)
	local mask = Physics.GROUP_ACTORS + Physics.GROUP_PLAYERS + Physics.GROUP_ITEMS
	Coroutine(function(t)
		local apply = function(r)
			-- Calculate the damage.
			Combat:apply_melee_impact(attacker, weapon, r.point, r.object, r.tile)
			return true
		end
		local cast = function(rel)
			-- Get the attack ray.
			local src,dst = attacker:get_attack_ray(rel)
			-- Cast from the previous point.
			local r = prev and Physics:cast_ray(prev, dst, mask)
			if r then return apply(r) end
			prev = dst
		end
		-- Play the attack effect.
		Server:object_effect(attacker, "swing1")
		Server:object_event(attacker, "object attack", {move = move, variant = math.random(0, 255)})
		-- Cast a curve against actors and items.
		for i = 1,4 do
			Coroutine:sleep(attacker.spec.timing_attack_melee * 0.02 / 4)
			if cast(path[i]) then return end
		end
		-- Cast a straight ray against everything.
		local src,dst = attacker:get_attack_ray()
		local r = Physics:cast_ray(src, dst)
		if r then return apply(r) end
	end)
	return move
end

Actionspec{
	name = "right melee",
	charge_start = function(user)
		local move = choose_move(user)
		user:animate("charge " .. move, true)
		user.attack_charge = Program:get_time()
		user.attack_charge_anim = "right melee"
		user.attack_charge_move = move
	end,
	charge_end = function(user)
		perform_attack(user, user.attack_charge_move)
		user:attack_charge_cancel()
		user.auto_attack = nil
		user.cooldown = (user.cooldown or 0) + 0.8
	end,
	charge_cancel = function(user)
		user:attack_charge_cancel(true)
		user.auto_attack = nil
		user.cooldown = 0.4
	end,
	func = function(feat, info, args)
		perform_attack(args.user, choose_move(args.user))
	end}
