Feateffectspec{
	name = "mindless march",
	categories =
	{
		["harmful"] = true,
		["push"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["touch spell"] = true
	},
	description = "Forces the target to march forward",
	effect = "light1",
	icon = "modifier-light",
	influences = {["mindless march"] = 30},
	projectile = "magicmissile1",
	required_stats = {["willpower"] = 5},
	modifier = function(self, mod, secs)
		mod.object:face_point{point = mod.args.position}
		mod.object:set_movement(1)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		if args.object.spec.type ~= "actor" then return end
		args.object:inflict_modifier("push", 10)
	end}
