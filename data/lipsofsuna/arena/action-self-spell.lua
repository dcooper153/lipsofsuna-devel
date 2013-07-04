local Damage = require("arena/damage")

-- Spell on self.
-- At the specific time into the attack animation, the effects of the
-- feat are applied to the attacker herself.
Actionspec{
	name = "self spell",
	categories =
	{
		["enchantment"] = true,
		["self spell"] = true
	},
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_self * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell self", true)
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
		-- Apply the modifiers.
		local damage = Damage()
		damage:add_spell_modifiers(modifiers)
		damage:apply_defender_vulnerabilities(action.object)
		Main.combat_utils:apply_damage_to_actor(action.object, action.object, damage)
	end,
	get_score = function(action)
		return 1
	end}
