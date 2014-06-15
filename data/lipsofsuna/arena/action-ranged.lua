local ActionSpec = require("core/specs/action")
local Damage = require("core/combat/damage")
local ProjectileController = require("arena/projectile-controller")

ActionSpec{
	name = "ranged",
	categories = { ["ranged"] = true },
	start = function(action, move)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Check for weapon and ammo.
		local weapon,ammo = Main.combat_utils:count_ranged_ammo_of_actor(action.object)
		if not ammo or ammo == 0 then
			if weapon then action.object:send_message("You have no ammo left.") end
			action.object.cooldown = 0.4
			return
		end
		-- Start the charge animation.
		action.object:animate("charge stand", true)
		-- Enable effect-over-time updates.
		action.charge_value = 0
		action.weapon = weapon
		action.delay = (weapon.spec.timings["fire"] or 0) * 0.02
		action.released = false
		action.time = 0
		return true
	end,
	update = function(action, secs)
		-- Check for cancel.
		local weapon = action.object:get_weapon()
		if weapon ~= action.weapon then
			action.cancel = true
		end
		if action.cancel then
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Check for charge finish.
		if not action.object.control_right then
			action.released = true
		end
		if action.released then
			-- Wait for the launch.
			action.object.cooldown = 1
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
			local projectile = ammo:split()
			-- Play the attack effect.
			local effect = weapon.spec:get_effect("ranged attack")
			if effect then
				Main.vision:object_effect(action.object, effect.name)
			end
			Main.vision:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
			-- Calculate the base damage.
			local damage = Damage()
			damage:add_item_modifiers(weapon, action.object.skills)
			damage:add_item_modifiers(projectile, action.object.skills)
			damage:add_knockback()
			damage:apply_attacker_charge(action.charge_value)
			-- Fire the projectile.
			local controller = ProjectileController(action.object, projectile, damage, 20, true)
			controller:attach()
			return
		end
		-- Continue charging.
		action.charge_value = action.charge_value + secs
		action.object.cooldown = 1
		return true
	end,
	get_range = function(action)
		return 20
	end,
	get_score = function(action)
		return 1
	end}
