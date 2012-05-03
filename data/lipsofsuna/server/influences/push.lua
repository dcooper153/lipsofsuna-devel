Feateffectspec:extend{
	name = "mindless march",
	modifier = function(self, mod, secs)
		mod.object:face_point{point = mod.args.position}
		mod.object:set_movement(1)
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	ranged = function(self, args)
		if not args.object then return end
		if args.object.spec.type ~= "species" then return end
		args.object:inflict_modifier("push", 10)
	end}
