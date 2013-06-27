local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local ProjectileController = require("core/server/projectile-controller")

Actionspec{
	name = "ranged",
	start = function(action)
		-- Check for weapon and ammo.
		local weapon,ammo = Main.combat_utils:count_ranged_ammo_of_actor(action.object)
		if not ammo or ammo == 0 then
			if weapon then action.object:send_message("You have no ammo left.") end
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Enable effect-over-time updates.
		action.weapon = weapon
		action.delay = (weapon.spec.timings["fire"] or 0) * 0.02
		action.time = 0
		return true
	end,
	update = function(action, secs)
		-- TODO: Wait for charge end.
		action.object.cooldown = 1
		-- Wait for the launch.
		action.time = action.time + secs
		if action.time < action.delay then return true end
		-- Split the ammo.
		local weapon,ammo = Main.combat_utils:split_ranged_ammo_of_actor(action.object)
		if not ammo then
			if weapon then action.object:send_message("You have no ammo left.") end
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Play the attack effect.
		if weapon.effect_attack then
			Server:object_effect(action.object, weapon.effect_attack)
		end
		Server:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Fire the projectile.
		local projectile = ammo:split()
		local damage = Combat:calculate_ranged_damage(action.object, projectile)
		local controller = ProjectileController(action.object, projectile, damage, 20, true)
		controller:attach()
	end}
