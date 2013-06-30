-- Dig terrain.
Feateffectspec{
	name = "dig",
	categories =
	{
		["dig"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true
	},
	description = "Fire a digging ray",
	effect = "spell1",
	icon = "modifier-earthmove",
	influences = {["dig"] = 1},
	projectile = "fireball1", -- FIXME
	required_stats = {["willpower"] = 3},
	touch = function(self, args)
		if not args.tile then return end
		if not Voxel:damage(args.owner, args.tile) then return end
		if not args.projectile then return end
		args.projectile.tiles_digged = (args.projectile.tiles_digged or 0) + 1
		if args.projectile.tiles_digged < 5 then return true end
	end}
