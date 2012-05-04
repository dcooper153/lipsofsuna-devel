Actionspec:extend{
	name = "drink",
	func = function(self, user)
		-- Play the use effect.
		-- FIXME: This is sort of redundant due to spell effects making noise already.
		if self.spec.effect_use then
			Effect:play{effect = self.spec.effect_use, object = user}
		end
		-- Apply the spell effects.
		local args = {object = user, owner = user, point = self.position}
		local feat = Feat{animation = "spell on touch", effects = {{nil,1}}}
		for k,v in pairs(self.spec.potion_effects) do
			feat.effects[1][1] = k
			feat:apply(args)
		end
		-- Replace the potion with an empty bottle.
		self:subtract(1)
		user.inventory:merge_or_drop_object(Item{spec = Itemspec:find{name = "empty bottle"}})
	end}
