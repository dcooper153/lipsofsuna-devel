local Feat = require("arena/feat")
local Item = require("core/objects/item")

Actionspec{
	name = "drink",
	label = "Drink",
	start = function(action, item)
		if not item then return end
		-- Play the use effect.
		-- FIXME: This is sort of redundant due to spell effects making noise already.
		Server:object_effect(action.object, item.spec.effect_use)
		-- Apply the spell effects.
		local args = {object = action.object, owner = action.object, point = action.object:get_position()}
		local feat = Feat("spell on touch", {{nil,1}})
		for k,v in pairs(item.spec.potion_effects) do
			feat.effects[1][1] = k
			feat:apply(args)
		end
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
