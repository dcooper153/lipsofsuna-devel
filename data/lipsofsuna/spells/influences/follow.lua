Feateffectspec:extend{
	name = "follow",
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
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
