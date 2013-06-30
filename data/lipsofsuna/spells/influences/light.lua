-- Increase light duration.
Feateffectspec{
	name = "light",
	categories =
	{
		["beneficial"] = true,
		["light"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true
	},
	description = "Illuminate your surroundings",
	effect = "light1",
	icon = "modifier-light",
	influences = {["light"] = 60},
	required_stats = {["willpower"] = 5},
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("light", args.value)
	end}
