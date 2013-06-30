-- Increase sanctuary duration.
Feateffectspec{
	name = "sanctuary",
	actions =
	{
		["self spell"] = true,
		["touch spell"] = true
	},
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "sanctuary1",
	influences = {["sanctuary"] = 30},
	required_stats = {["willpower"] = 20},
	modifier = function(self, mod, secs)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("sanctuary", args.value)
	end}
