Actionspec{
	name = "ranged",
	charge_start = function(user)
		-- Start charging the attack.
		user:animate("charge stand", true)
		user.attack_charge = Program.time
		user.attack_charge_anim = "ranged"
	end,
	charge_end = function(user)
		-- Initialize the feat.
		local feat = Feat{animation = "ranged"}
		feat:add_best_effects{user = user}
		-- Perform the feat.
		local res = feat:perform{stop = false, user = user}
		user:attack_charge_cancel(not res)
	end,
	charge_cancel = function(user)
		user:attack_charge_cancel(true)
		user.auto_attack = nil
		user.cooldown = math.max(user.cooldown or 0, 0.3)
	end,
	func = function(feat, info, args)
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
