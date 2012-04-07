Feateffectspec:extend{
	name = "mindless march",
	modifier = function(self, object, args, secs)
		object:face_point{point = args.a.position}
		object:set_movement(1)
		return {st=args.st - secs,a=args.a}
	end,
	ranged = function(self, args)
		if not args.object then return end
		if args.object.spec.type ~= "species" then return end
		args.object:inflict_modifier("push", 10)
	end}
