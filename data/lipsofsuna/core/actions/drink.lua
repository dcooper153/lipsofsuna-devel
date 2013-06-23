local Feat = require("arena/feat")
local Item = require("core/objects/item")

Actionspec{
	name = "drink",
	label = "Drink",
	func = function(self, user)
		-- Play the use effect.
		-- FIXME: This is sort of redundant due to spell effects making noise already.
		Server:object_effect(user, self.spec.effect_use)
		-- Apply the spell effects.
		local args = {object = user, owner = user, point = self:get_position()}
		local feat = Feat("spell on touch", {{nil,1}})
		for k,v in pairs(self.spec.potion_effects) do
			feat.effects[1][1] = k
			feat:apply(args)
		end
		-- Replace the potion with an empty bottle.
		self:subtract(1)
		local item = Item(user.manager)
		item:set_spec(Itemspec:find_by_name("empty bottle"))
		user.inventory:merge_or_drop_object(item)
		-- Log the action.
		Server.events:notify_action("eat", user)
	end}
