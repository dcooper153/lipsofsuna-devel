-- Ranged attack.
-- A projectile is fired at the specific time into the attack
-- animation. The collision callback of the projectile takes
-- care of damaging the hit object or tile.
Actionspec{name = "ranged", func = function(feat, info, args)
	Coroutine(function(t)
		if args.weapon then
			local frames = args.weapon.spec.timings["fire"]
			if frames then
				Coroutine:sleep(frames * 0.02)
			end
		end
		feat:play_effects(args)
		for name,count in pairs(info.required_ammo) do
			local ammo = args.user.inventory:split_object_by_name(name, count)
			if ammo then
				ammo:fire{
					charge = args.charge,
					collision = true,
					feat = feat,
					owner = args.user,
					speedline = true,
					weapon = args.weapon}
				return
			end
		end
	end)
end}
