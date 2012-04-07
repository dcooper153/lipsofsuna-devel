-- Increase sanctuary duration.
Feateffectspec:extend{
	name = "regeneration",
	modifier = function(self, object, args, secs)
		-- Update the timer.
		object.regeneration_timer = (object.regeneration_timer or 0) + secs
		-- Heal the object every second.
		if object.regeneration_timer > 1 then
			object:damaged{amount = -math.random(2,4), type = "physical"}
			object.regeneration_timer = object.regeneration_timer - 1
		end
		-- End after the timeout.
		return {st=args.st - secs}
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("regeneration", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
