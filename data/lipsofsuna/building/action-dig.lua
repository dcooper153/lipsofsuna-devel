local Combat = require("core/server/combat")
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
		local src,dst = action.object:get_attack_ray()
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if r then
			local weapon = action.object:get_weapon()
			if not r.object then
				-- Dig the terrain.
				Main.building_utils:destroy_terrain_stick(action.object, r.point, r.tile, 1)
				-- Damage the weapon.
				if weapon and weapon.spec.damage_mining then
					if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
						action.object:send_message("The " .. weapon.spec.name .. " broke!")
					end
				end
			else
				local target = r.object and Main.objects:find_by_id(r.object)
				Combat:apply_melee_impact(action.object, weapon, r.point, target, r.tile)
			end
		end
	end}
