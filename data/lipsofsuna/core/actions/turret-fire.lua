local ProjectileController = require("arena/projectile-controller")

Actionspec{
	name = "turret fire",
	categories = { ["ranged"] = true },
	start = function(action)
		local attacker = action.object
		-- Split the ammo.
		local ammo = attacker.inventory:split_object_by_name("bullet", 1)
		if not ammo then return end
		-- Play the attack effect.
		Main.vision:object_effect(attacker, "musket1")
		Main.vision:object_event(attacker, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Fire the projectile.
		local projectile = ammo:split()
		local damage = Main.combat_utils:calculate_ranged_damage(attacker, projectile)
		local controller = ProjectileController(attacker, projectile, damage, 20, true)
		controller:attach()
	end,
	get_range = function(action)
		return 20
	end,
	get_score = function(action)
		return 1
	end}
