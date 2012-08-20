local Combat = require("core/server/combat")
local ProjectileController = require("core/server/projectile-controller")

local perform_attack = function(attacker)
	-- Split the ammo.
	local ammo = attacker.inventory:split_object_by_name("bullet", 1)
	if not ammo then return end
	-- Play the attack effect.
	Server:object_effect(attacker, "musket1")
	Vision:event{type = "object attack", object = attacker, move = "stand", variant = math.random(0, 255)}
	-- Fire the projectile.
	local projectile = ammo:split()
	local damage = Combat:calculate_ranged_damage(attacker, projectile)
	local controller = ProjectileController(attacker, projectile, damage, 20, true)
	controller:attach()
end

Actionspec{
	name = "turret fire",
	func = function(feat, info, args)
		perform_attack(args.user)
	end}
