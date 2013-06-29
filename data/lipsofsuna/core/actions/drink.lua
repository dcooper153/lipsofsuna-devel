local Damage = require("arena/damage")
local Item = require("core/objects/item")

Actionspec{
	name = "drink",
	label = "Drink",
	start = function(action, item)
		if not item then return end
		-- Play the use effect.
		Server:object_effect(action.object, item.spec.effect_use)
		-- Apply the spell effects.
		local damage = Damage()
		damage:add_spell_influences(item.spec.potion_effects)
		damage:apply_defender_vulnerabilities(action.object)
		Main.combat_utils:apply_damage_to_actor(action.object, action.object, damage)
		-- Replace the potion with an empty bottle.
		item:subtract(1)
		local item1 = Item(action.object.manager)
		item1:set_spec(Itemspec:find_by_name("empty bottle"))
		action.object.inventory:merge_or_drop_object(item1)
		-- Log the action.
		if Server.events then
			Server.events:notify_action("eat", action.object)
		end
	end}
