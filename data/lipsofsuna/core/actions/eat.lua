local Feat = require("arena/feat")

Actionspec{
	name = "eat",
	label = "Eat",
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
		-- Remove the item.
		self:subtract(1)
		-- Log the action.
		Server.events:notify_action("eat", user)
	end}
