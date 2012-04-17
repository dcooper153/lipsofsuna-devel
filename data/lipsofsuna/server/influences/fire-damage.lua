-- Decrease health.
Feateffectspec:extend{
	name = "fire damage",
	touch = function(self, args)
		if not args.object then return end
		-- Randomize the amount.
		local val = args.value
		val = math.min(-1, val + val * 0.5 * math.random())
		-- Apply unless friendly fire.
		if not args.owner.client or not args.object.client then
			args.object:damaged{amount = -val, point = args.point, type = "fire"}
		end
		-- Anger hurt creatures.
		if val > 0 then
			args.object:add_enemy(args.owner)
		end
	end,
	ranged = function(self, args)
		self:touch(args)
	end}