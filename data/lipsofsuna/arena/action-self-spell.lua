local Damage = require("arena/damage")

-- Spell on self.
-- At the specific time into the attack animation, the effects of the
-- feat are applied to the attacker herself.
Actionspec{
	name = "self spell",
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_ranged * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell self", true)
		return true
	end,
	update = function(action, secs)
		-- Apply the casting delay.
		action.object.cooldown = 0.4
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Get the influences.
		local influences = Main.combat_utils:get_spell_influences_for_item(action.weapon)
		if not influences then return end
		-- TODO: Subtract stats.
		-- Apply the influences.
		local damage = Damage()
		damage:add_spell_influences(influences)
		damage:apply_defender_vulnerabilities(action.object)
		Main.combat_utils:apply_damage_to_actor(action.object, action.object, damage)
	end}
