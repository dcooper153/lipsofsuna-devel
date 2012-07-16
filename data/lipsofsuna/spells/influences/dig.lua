-- Dig terrain.
Feateffectspec:extend{
	name = "dig",
	touch = function(self, args)
		if not args.tile then return end
		if not Voxel:damage(args.owner, args.tile) then return end
		return true
	end,
	ranged = function(self, args)
		if not self:touch(args) then return end
		args.projectile.tiles_digged = (args.projectile.tiles_digged or 0) + 1
		if args.projectile.tiles_digged < 5 then return true end
	end}
