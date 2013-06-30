local Combat = require("core/server/combat")
local Physics = require("system/physics")

Actionspec{
	name = "melee",
	categories = { ["melee"] = true },
	start = function(action, move)
		if action.object.cooldown then return end
		-- Initialize timing.
		action.frame = 1
		action.time = 0
		action.duration = action.object.spec.timing_attack_melee * 0.02
		-- Choose the weapon sweep path.
		if not move then
			move = Main.combat_utils:get_melee_move_of_actor(action.object)
		end
		local paths = {
			["stand"] = {Vector(0.1, 0, 0.75), Vector(-0.1, 0, 0.5), Vector(0.05, 0, 0.25), Vector(-0.05, 0, 0)},
			["left"] = {Vector(0.5, 0.1, 1), Vector(0.25, 0.05, 0.7), Vector(0, 0, 0.5), Vector(-0.25, 0, 0.7)},
			["right"] = {Vector(0.5, 0.1, 0.7), Vector(0.1, 0.05, 0.5), Vector(-0.2, 0, 0.6), Vector(-0.5, 0, 1)},
			["back"] = {Vector(0.1, 0.2, 1), Vector(0.05, 0.2, 0.75), Vector(0, 0.1, 0.5), Vector(0, -0.1, 0.25)},
			["front"] = {Vector(0.3, -0.1, 0.8), Vector(0.2, -0.1, 0.55), Vector(0.1, -0.05, 0.3), Vector(0, 0.05, 0.15)}}
		action.path = paths[move]
		if not action.path then return end
		action.prev = Main.combat_utils:get_actor_attack_point(action.object, action.path[1])
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		Main.vision:object_event(action.object, "object attack", {move = move, variant = math.random(0, 255)})
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		local apply = function(r)
			local weapon = action.object:get_weapon()
			local target = r.object and Main.objects:find_by_id(r.object)
			Combat:apply_melee_impact(action.object, action.weapon, r.point, target, r.tile)
		end
		-- Wait for the next sweep.
		action.object.cooldown = 0.3
		action.time = action.time + secs
		local frame = action.time / action.duration * #action.path + 1
		if frame <= action.frame then return true end
		action.frame = action.frame + 1
		-- Perform the sweep.
		local dst = Main.combat_utils:get_actor_attack_point(action.object, action.path[action.frame])
		local r = Physics:cast_ray(action.prev, dst, nil, {action.object.physics})
		action.prev = dst
		if r then return apply(r) end
		if action.path[action.frame] then return true end
		-- Cast a straight ray as a fallback.
		local src,dst = action.object:get_attack_ray()
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if r then apply(r) end
	end,
	get_score = function(action)
		return 1
	end}
