local Combat = require("core/server/combat")
local ProjectileController = require("arena/projectile-controller")

local perform_attack = function(attacker)
	-- Split the ammo.
	local ammo = attacker.inventory:split_object_by_name("bullet", 1)
	if not ammo then return end
	-- Play the attack effect.
	Main.vision:object_effect(attacker, "musket1")
	Main.vision:object_event(attacker, "object attack", {move = "stand", variant = math.random(0, 255)})
	-- Fire the projectile.
	local projectile = ammo:split()
	local damage = Combat:calculate_ranged_damage(attacker, projectile)
	local controller = ProjectileController(attacker, projectile, damage, 20, true)
	controller:attach()
end

Actionspec{
	name = "turret fire",
	start = function(action)
		perform_attack(action.object)
	end}
