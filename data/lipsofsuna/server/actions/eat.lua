Actionspec:extend{
	name = "eat",
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
		-- Remove the item.
		self:subtract(1)
		-- Log the action.
		Globaleventmanager:notify_action("eat", user)
	end}
