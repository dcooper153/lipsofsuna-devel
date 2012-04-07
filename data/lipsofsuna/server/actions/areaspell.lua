Actionspec{
	name = "area spell",
	func = function(feat, info, args)
		Coroutine(function(t)
			Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
			feat:play_effects(args)
			local spec = Spellspec:find{name = "firewall1"} --FIXME
			local spell = AreaSpell{duration = 15, feat = feat, owner = args.user,
				position = args.user.position, realized = true, spec = spec}
		end)
	end}
