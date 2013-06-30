-- Decrease health.
Feateffectspec{
	name = "fire damage",
	categories =
	{
		["fire"] = true,
		["harmful"] = true,
		["melee"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Inflict fire damage",
	effect = "explosion1",
	icon = "modifier-fireball",
	influences = {["fire damage"] = 5},
	projectile = "fireball1",
	required_stats = {["willpower"] = 1},
	touch = function(self, args)
		if not args.object then return end
		-- Randomize the amount.
		local val = args.value
		val = math.max(1, val + val * 0.5 * math.random())
		-- Apply unless friendly fire.
		if not args.owner.client or not args.object.client then
			args.object:damaged{amount = val, point = args.point, type = "fire"}
		end
		-- Anger hurt actors.
		if val > 0 then
			args.object:add_enemy(args.owner)
		end
	end}
