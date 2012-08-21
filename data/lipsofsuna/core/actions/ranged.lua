local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local ProjectileController = require("core/server/projectile-controller")

local perform_attack = function(attacker, weapon)
	if not weapon then return end
	if not weapon.spec.ammo_type then return end
	Coroutine(function(t)
		-- Delay the launch.
		local frames = weapon.spec.timings["fire"]
		if frames then Coroutine:sleep(frames * 0.02) end
		-- Split the ammo.
		local ammo = attacker.inventory:split_object_by_name(weapon.spec.ammo_type, 1)
		if not ammo then return end
		-- Play the attack effect.
		if weapon.effect_attack then
			Server:object_effect(attacker, weapon.effect_attack)
		end
		Server:object_event(attacker, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Fire the projectile.
		local projectile = ammo:split()
		local damage = Combat:calculate_ranged_damage(attacker, projectile)
		local controller = ProjectileController(attacker, projectile, damage, 20, true)
		controller:attach()
	end)
end

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
			perform_attack(user, weapon)
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
		perform_attack(args.user, args.weapon)
	end}
