Actionspec{
	name = "turret fire",
	func = function(feat, info, args)
		feat:play_effects(args)
		local ammo = args.user.inventory:split_object_by_name("bullet", 1)
		if not ammo then return end
		ammo:fire{
			charge = 10,
			collision = true,
			feat = feat,
			owner = args.user,
			speedline = true}
	end}
