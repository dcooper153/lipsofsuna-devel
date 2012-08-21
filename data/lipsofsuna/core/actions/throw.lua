local BoomerangController = require("core/server/boomerang-controller")
local Combat = require("core/server/combat")
local Coroutine = require("system/coroutine")
local ProjectileController = require("core/server/projectile-controller")

local perform_attack = function(attacker, weapon)
	local charge = 1 + 2 * attacker:get_attack_charge()
	Coroutine(function(t)
		Coroutine:sleep(attacker.spec.timing_attack_throw * 0.02)
		-- Play the attack effect.
		Server:object_effect(attacker, "swing1")
		Server:object_event(attacker, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Fire the projectile.
		local projectile = weapon:split()
		local damage = Combat:calculate_ranged_damage(attacker, projectile)
		local controller
		if projectile.spec.categories["boomerang"] then
			controller = BoomerangController(attacker, projectile, damage)
		else
			controller = ProjectileController(attacker, projectile, damage, 10 * charge)
		end
		controller:attach()
	end)
end

Actionspec{
	name = "throw",
	charge_start = function(user)
		user:animate("charge stand", true)
		user.attack_charge = Program:get_time()
		user.attack_charge_anim = "throw"
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
	func = function(feat, info, args)
		perform_attack(args.user, args.weapon)
	end}
