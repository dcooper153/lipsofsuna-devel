Actionspec:extend{
	name = "drink",
	func = function(self, user)
		-- FIXME: Abusing the old use actions.
		-- TODO: Use feat effects instead? Could do multiple potion effects that way too.
		local action = Actionspec:find{name = self.spec.action_use}
		if not action then return end
		if self.spec.effect_use then
			Effect:play{effect = self.spec.effect_use, object = user}
		end
		action.func(self, user)
	end}
