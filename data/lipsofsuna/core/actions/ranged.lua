local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local ProjectileController = require("core/server/projectile-controller")

Actionspec{
	name = "ranged",
	charge_start = function(user)
		-- Check for weapon and ammo.
		local weapon,ammo = Main.combat_utils:count_ranged_ammo_of_actor(user)
		if not ammo or ammo == 0 then
			if weapon then user:send_message("You have no ammo left.") end
			return
		end
		-- Start charging.
		user:animate("charge stand", true)
		user.attack_charge = Program:get_time()
		user.attack_charge_anim = "ranged"
	end,
	charge_end = function(user)
		user:attack_charge_cancel()
		user:action("ranged")
		user.auto_attack = nil
		user.cooldown = (user.cooldown or 0) + 1
	end,
	charge_cancel = function(user)
		user:attack_charge_cancel(true)
		user.auto_attack = nil
		user.cooldown = math.max(user.cooldown or 0, 0.3)
	end,
	func = function(feat, info, args)
		args.user:action("ranged")
	end,

	start = function(action)
		-- Check for weapon and ammo.
		local weapon,ammo = Main.combat_utils:count_ranged_ammo_of_actor(action.object)
		if not ammo or ammo == 0 then
			if weapon then action.object:send_message("You have no ammo left.") end
			action.object:attack_charge_cancel(true)
			return
		end
		-- Enable effect-over-time.
		action.weapon = weapon
		action.delay = (weapon.spec.timings["fire"] or 0) * 0.02
		action.time = 0
		return true
	end,
	update = function(action, secs)
		-- Wait for the launch.
		action.time = action.time + secs
		if action.time < action.delay then return true end
		-- Split the ammo.
		local weapon,ammo = Main.combat_utils:split_ranged_ammo_of_actor(action.object)
		if not ammo then
			if weapon then action.object:send_message("You have no ammo left.") end
			action.object:attack_charge_cancel(true)
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
