-- Increase sanctuary duration.
Feateffectspec:extend{
	name = "sanctuary",
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("sanctuary", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
