Feateffectspec{
	name = "curse",
	categories =
	{
		["harmful"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Damages the maximum health and willpower of the target",
	effect = "spell1",
	icon = "firewall", --FIXME
	influences = {["curse"] = 60},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
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
	end}
