local Actor = require("core/objects/actor")

Feateffectspec:extend{
	name = "fire elemental",
	touch = function(self, args)
		-- Find an empty ground spot.
		if not args.object then return end
		local ctr = Utils:find_summon_point(args.object:get_position())
		if not ctr then return end
		-- Create the summon.
		local spec = Actorspec:find{name = "fire elemental"}
		if not spec then return end
		local summon = Actor{
			spec = spec,
			position = ctr,
			random = true,
			realized = true}
		summon.summon_owner = args.owner
		summon.summon_timer = args.value
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
