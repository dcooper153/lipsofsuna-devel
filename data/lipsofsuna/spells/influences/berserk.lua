-- Increase berserk duration.
Feateffectspec{
	name = "berserk",
	categories =
	{
		["beneficial"] = true,
		["berserk"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Inflict additional damage when in low health",
	effect = "berserk1",
	icon = "modifier-berserk",
	influences = {["berserk"] = 60},
	required_stats = {["willpower"] = 5},
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("berserk", args.value)
	end}
