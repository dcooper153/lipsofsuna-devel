local ActionSpec = require("core/specs/action")
local Damage = require("core/combat/damage")

ActionSpec{
	name = "drink",
	label = "Drink",
	start = function(action, item)
		if not item then return end
		-- Play the use effect.
		local effect = item.spec:get_effect("use")
		if effect then
			Main.vision:object_effect(action.object, effect.name)
		end
		-- Apply the spell effects.
		local damage = Damage()
		damage:add_spell_modifiers(item.spec.potion_effects)
		damage:apply_defender_vulnerabilities(action.object)
		Main.combat_utils:apply_damage_to_actor(action.object, action.object, damage)
		-- Replace the potion with an empty bottle.
		item:subtract(1)
		local item1 = action.object.manager:create_object_by_spec("Item", "empty bottle")
		if item1 then
			action.object.inventory:merge_or_drop_object(item1)
		end
		-- Log the action.
		if Server.events then
			Server.events:notify_action("eat", action.object)
		end
	end}
