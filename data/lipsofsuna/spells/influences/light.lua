-- Increase light duration.
Feateffectspec:extend{
	name = "light",
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("light", args.value)
	end}
