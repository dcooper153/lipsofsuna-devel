Feateffectspec{
	name = "restore willpower",
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal", --FIXME
	influences = {["restore willpower"] = 15},
	required_stats = {["willpower"] = 5},
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
			local value = args.object.stats:get_value("willpower")
			if not value then return end
			args.object.stats:set_value("willpower", value + val)
		end
		-- Anger hurt actors.
		if val < 0 then
			args.object:add_enemy(args.owner)
		end
	end}
