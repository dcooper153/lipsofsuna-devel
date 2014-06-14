local ActionSpec = require("core/specs/action")
local Damage = require("arena/damage")
local Physics = require("system/physics")

ActionSpec{
	name = "touch spell",
	categories =
	{
		["enchantment"] = true,
		["touch spell"] = true
	},
	description = "Cast a spell on a target right in front of you.",
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_touch * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell touch", true)
		return true
	end,
	update = function(action, secs)
		-- Apply the casting delay.
		action.object.cooldown = 0.4
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Get the modifiers.
		local modifiers = Main.combat_utils:get_spell_modifiers_for_item(action.weapon)
		if not modifiers then return end
		-- Subtract stats.
		local ok,stat = Main.combat_utils:subtract_modifier_stats_for_actor(action.object, modifiers)
		if not ok then
			action.object:send_message("You do not have have enough " .. stat .. ".")
			return
		end
		-- Cast a straight attack ray.
		local src,dst = Main.combat_utils:get_attack_ray_for_actor(action.object)
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if not r then return end
		-- Apply the damage.
		local target = r.object and Main.objects:find_by_id(r.object)
		if target then
			local damage = Damage()
			damage:add_spell_modifiers(modifiers)
			damage:apply_defender_vulnerabilities(target)
			Main.combat_utils:apply_damage_to_actor(action.object, target, damage, r.point)
		else
			-- TODO: Damage terrain
		end
	end,
	get_score = function(action)
		return 1
	end}
