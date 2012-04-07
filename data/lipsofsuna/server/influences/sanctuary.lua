-- Increase sanctuary duration.
Feateffectspec:extend{
	name = "sanctuary",
	modifier = function(self, object, args, secs)
		return {st=args.st - secs}
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("sanctuary", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
