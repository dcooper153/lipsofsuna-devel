-- Teleport to Lips.
Feateffectspec:extend{
	name = "travel",
	touch = function(self, args)
		if not args.object then return end
		args.object:teleport{region = "Lips"}
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
