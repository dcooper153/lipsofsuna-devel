Actionspec{name = "missile spell", func = function(feat, info, args)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
		feat:play_effects(args)
		local spec = Spellspec:find{name = "magicmissile1"}
		MissileSpell{feat = feat, owner = args.user, spec = spec}
	end)
end}
