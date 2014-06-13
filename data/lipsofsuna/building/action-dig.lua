local Physics = require("system/physics")

Actionspec{
	name = "dig",
	start = function(action, move)
		if action.object.cooldown then return end
		-- Initialize timing.
		action.timer = 0
		action.delay = action.object.spec.timing_attack_melee * 0.02
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		Main.vision:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Apply the delay.
		action.object.cooldown = 0.4
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Perform the sweep.
		local src,dst = Main.combat_utils:get_attack_ray_for_actor(action.object, 0)
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if r then
			local attacker = action.object
			local defender = r.object and Main.objects:find_by_id(r.object)
			local weapon = attacker:get_weapon()
			if not defender then
				-- Dig the terrain.
				Main.building_utils:destroy_terrain_stick(attacker, r.point, r.tile, 1)
				-- Damage the weapon.
				if weapon and weapon.spec.damage_mining then
					if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
						action.object:send_message("The " .. weapon.spec.name .. " broke!")
					end
				end
			else
				local damage = Damage()
				damage:add_item_or_unarmed_modifiers(weapon, attacker.skills)
				damage:add_knockback()
				damage:apply_attacker_physical_modifiers(attacker)
				damage:apply_attacker_charge(attacker:get_attack_charge())
				damage:apply_defender_armor(defender)
				damage:apply_defender_blocking(defender)
				damage:apply_defender_vulnerabilities(defender)
				Main.combat_utils:apply_damage_to_actor(attacker, defender, damage, r.point)
			end
		end
	end}
