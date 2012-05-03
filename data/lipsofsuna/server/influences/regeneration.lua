-- Increase sanctuary duration.
Feateffectspec:extend{
	name = "regeneration",
	modifier = function(self, mod, secs)
		-- Update the timer.
		mod.timer = mod.timer + secs
		-- Heal the object every second.
		if mod.timer > 1 then
			mod.object:damaged{amount = -math.random(2,4), type = "physical"}
			mod.timer = mod.timer - 1
		end
		-- End after the timeout.
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("regeneration", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
