local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local ProjectileController = require("core/server/projectile-controller")

Actionspec{
	name = "ranged",
	charge_start = function(user)
		user:animate("charge stand", true)
		user.attack_charge = Program:get_time()
		user.attack_charge_anim = "ranged"
	end,
	charge_end = function(user)
		local weapon = user:get_weapon()
		if weapon then
			user:action("ranged", weapon)
			user:attack_charge_cancel()
		else
			user:attack_charge_cancel(true)
		end
		user.auto_attack = nil
		user.cooldown = (user.cooldown or 0) + 1
	end,
	charge_cancel = function(user)
		user:attack_charge_cancel(true)
		user.auto_attack = nil
		user.cooldown = math.max(user.cooldown or 0, 0.3)
	end,
	func = function(feat, info, args)
		args.user:action("ranged", args.weapon)
	end,

	start = function(action, weapon)
		if not weapon then return end
		if not weapon.spec.ammo_type then return end
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
		local ammo = action.object.inventory:split_object_by_name(action.weapon.spec.ammo_type, 1)
		if not ammo then return end
		-- Play the attack effect.
		if action.weapon.effect_attack then
			Server:object_effect(action.object, action.weapon.effect_attack)
		end
		Server:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Fire the projectile.
		local projectile = ammo:split()
		local damage = Combat:calculate_ranged_damage(action.object, projectile)
		local controller = ProjectileController(action.object, projectile, damage, 20, true)
		controller:attach()
	end}
