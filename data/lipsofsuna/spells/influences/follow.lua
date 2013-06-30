Feateffectspec{
	name = "follow",
	categories =
	{
		["follow"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["touch spell"] = true
	},
	description = "Forces the target to follow you",
	effect = "light1",
	icon = "modifier-light",
	influences = {["follow"] = 30},
	required_stats = {["willpower"] = 5},
	projectile = "magicmissile1",
	modifier = function(self, mod, secs)
		mod.timer = mod.timer + secs
		if mod.timer > 1 then
			mod.object:face_point{point = mod.args.position}
			mod.object:set_movement(10)
			mod.object:climb()
			mod.timer = mod.timer - 1
		end
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("follow", args.value, args.owner)
	end}
