-- Increase berserk duration.
Feateffectspec:extend{
	name = "berserk",
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("berserk", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
