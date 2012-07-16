-- Decrease health.
Feateffectspec:extend{
	name = "cold damage",
	touch = function(self, args)
		if not args.object then return end
		-- Randomize the amount.
		local val = args.value
		val = math.max(1, val + val * 0.5 * math.random())
		-- Apply unless friendly fire.
		if not args.owner.client or not args.object.client then
			args.object:damaged{amount = val, point = args.point, type = "cold"}
		end
		-- Anger hurt actors.
		if val > 0 then
			args.object:add_enemy(args.owner)
		end
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
