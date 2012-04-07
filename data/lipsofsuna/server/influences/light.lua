-- Increase light duration.
Feateffectspec:extend{
	name = "light",
	modifier = function(self, object, args, secs)
		return {st=args.st - secs}
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("light", args.value)
	end}
