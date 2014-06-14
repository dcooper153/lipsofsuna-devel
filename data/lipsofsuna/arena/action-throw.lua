local ActionSpec = require("core/specs/action")
local BoomerangController = require("arena/boomerang-controller")
local ProjectileController = require("arena/projectile-controller")

ActionSpec{
	name = "throw",
	categories = { ["throw"] = true },
	start = function(action)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Get the projectile.
		local item = action.object:get_weapon()
		if not item then return end
		action.item = item
		-- Start the charge animation.
		action.object:animate("charge stand", true)
		-- Enable effect-over-time updates.
		action.charge_value = 0
		action.delay = action.object.spec.timing_attack_throw * 0.02
		action.timer = 0
		return true
	end,
	update = function(action, secs)
		-- Check for cancel.
		local item = action.object:get_weapon()
		if item ~= action.item then
			action.cancel = true
		end
		if action.cancel then
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Check for charge finish.
		action.object.cooldown = 1
		if not action.object.control_right then
			-- Wait for the release delay.
			action.timer = action.timer + secs
			if action.timer < action.delay then return true end
			-- Get the projectile.
			local projectile = action.item:split()
			local damage = Main.combat_utils:calculate_ranged_damage(action.object, projectile)
			-- Play the attack effect.
			Main.vision:object_effect(action.object, "swing1")
			Main.vision:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
			-- Fire the projectile.
			if projectile.spec.categories["boomerang"] then
				local controller = BoomerangController(action.object, projectile, damage)
				controller:attach()
			else
				local controller = ProjectileController(action.object, projectile, damage, 10 * action.charge_value)
				controller:attach()
			end
			return
		end
		-- Continue charging.
		action.charge_value = action.charge_value + secs
		return true
	end}
