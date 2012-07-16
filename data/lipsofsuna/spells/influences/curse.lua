Feateffectspec:extend{
	name = "curse",
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	modifier_attributes = function(self, mod, attr)
		attr.max_health = attr.max_health - 20
		attr.max_willpower = attr.max_willpower - 20
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("curse", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
