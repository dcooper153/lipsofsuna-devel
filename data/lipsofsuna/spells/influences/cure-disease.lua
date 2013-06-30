Feateffectspec{
	name = "cure disease",
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Cure the target of poison and diseases",
	effect = "spell1",
	icon = "modifier-heal", --FIXME
	influences = {["cure disease"] = 1},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	touch = function(self, args)
		if not args.object then return end
		args.object:remove_modifier("black haze")
		args.object:remove_modifier("poison")
	end}
