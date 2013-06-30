-- Teleport to Lips.
Feateffectspec{
	name = "travel",
	actions =
	{
		["self spell"] = true
	},
	categories =
	{
		["spell"] = true
	},
	description = "Fast travel to Lips",
	effect = "sanctuary1", -- FIXME
	icon = "sanctuary1", -- FIXME
	influences = {["travel"] = 30},
	required_stats = {["willpower"] = 10},
	touch = function(self, args)
		if not args.object then return end
		args.object:teleport{region = "Lips"}
	end}
