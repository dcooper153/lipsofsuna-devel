-- Increase or decrease health.
Feateffectspec:extend{
	name = "restore health",
	touch = function(self, args)
		if not args.object then return end
		-- Randomize the amount.
		local val = args.value
		if val < 0 then
			val = math.min(-1, val + val * 0.5 * math.random())
		else
			val = math.max(1, val + val * 0.5 * math.random())
		end
		-- Apply unless friendly fire.
		if val > 0 or not args.owner.client or not args.object.client then
			args.object:damaged{amount = -val, point = args.point, type = "physical"}
		end
		-- Anger hurt creatures.
		if val < 0 then
			args.object:add_enemy(args.owner)
		end
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
