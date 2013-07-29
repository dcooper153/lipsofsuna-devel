local Spell = require("core/objects/spell")

Actionspec{
	name = "ranged spell",
	categories =
	{
		["enchantment"] = true,
		["ranged spell"] = true
	},
	description = "Cast a spell on a target far ahead.",
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_ranged * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell ranged", true)
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
		-- Fire the spell.
		local spec = Spellspec:find_by_name("fireball1") --FIXME
		if not spec then return end
		local spell = Spell(action.object.manager)
		spell:set_spec(spec)
		spell:set_modifiers(modifiers)
		spell:set_owner(action.object)
		spell:fire()
	end,
	get_score = function(action)
		return 1
	end}
