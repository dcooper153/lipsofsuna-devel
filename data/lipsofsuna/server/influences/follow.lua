Feateffectspec:extend{
	name = "follow",
	modifier = function(self, object, args, secs)
		object.following_timer = (object.following_timer or 0) + secs
		if object.following_timer > 1 then
			object:face_point{point = args.a.position}
			object:set_movement(10)
			object:climb()
			object.following_timer = object.following_timer - 1
		end
		return {st=args.st - secs,a=args.a}
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("follow", args.value, args.owner)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
