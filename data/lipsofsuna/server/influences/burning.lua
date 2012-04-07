-- Sets the target on fire.
Feateffectspec:extend{
	name = "burning",
	modifier = function(self, object, args, secs)
		-- Update the burning timer.
		object.burning_timer = (object.burning_timer or 0) + secs
		-- Damage the object every second.
		if object.burning_timer > 1 then
			object:damaged{amount = math.random(4,7), type = "burning"}
			object.burning_timer = object.burning_timer - 1
		end
		-- End after the timeout.
		return {st=args.st - secs}
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("burning", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
