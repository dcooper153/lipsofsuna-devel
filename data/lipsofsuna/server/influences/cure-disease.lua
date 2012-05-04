Feateffectspec:extend{
	name = "cure disease",
	touch = function(self, args)
		if not args.object then return end
		args.object:remove_modifier("black haze")
		args.object:remove_modifier("poison")
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
