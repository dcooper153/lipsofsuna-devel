Actionspec{
	name = "throw",
	charge_start = function(user)
		-- Start charging the attack.
		user:animate("charge stand", true)
		user.attack_charge = Program.time
		user.attack_charge_anim = "throw"
	end,
	charge_end = function(user)
		-- Initialize the feat.
		local feat = Feat{animation = "throw"}
		feat:add_best_effects{user = user}
		-- Perform the feat.
		local res = feat:perform{stop = false, user = user}
		user:attack_charge_cancel(not res)
	end,
	func = function(feat, info, args)
		local charge = 1 + 2 * math.min(1, (args.charge or 0) / 2)
		Coroutine(function(t)
			Coroutine:sleep(args.user.spec.timing_attack_throw * 0.02)
			feat:play_effects(args)
			local proj = args.weapon:fire{
				charge = charge,
				collision = not args.weapon.spec.destroy_timer,
				feat = feat,
				owner = args.user,
				speed = 10 * charge,
				timer = args.weapon.spec.destroy_timer}
		end)
	end}
