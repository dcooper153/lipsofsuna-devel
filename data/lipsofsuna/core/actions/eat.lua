local Damage = require("arena/damage")

Actionspec{
	name = "eat",
	label = "Eat",
	start = function(action, item)
		if not item then return end
		-- Play the use effect.
		Server:object_effect(action.object, item.spec.effect_use)
		-- Apply the spell effects.
		local damage = Damage()
		damage:add_spell_influences(item.spec.potion_effects)
		damage:apply_defender_vulnerabilities(action.object)
		Main.combat_utils:apply_damage_to_actor(action.object, action.object, damage)
		-- Remove the item.
		item:subtract(1)
		-- Log the action.
		if Server.events then
			Server.events:notify_action("eat", action.object)
		end
	end}
