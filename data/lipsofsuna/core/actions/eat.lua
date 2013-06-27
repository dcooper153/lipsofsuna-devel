local Feat = require("arena/feat")

Actionspec{
	name = "eat",
	label = "Eat",
	func = function(action, item)
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
		-- Remove the item.
		item:subtract(1)
		-- Log the action.
		Server.events:notify_action("eat", action.object)
	end}
