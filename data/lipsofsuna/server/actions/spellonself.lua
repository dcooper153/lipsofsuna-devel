-- Spell on self.
-- At the specific time into the attack animation, the effects of the
-- feat are applied to the attacker herself.
Actionspec{name = "spell on self", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_spell_self * 0.02)
		feat:apply{
			charge = args.charge,
			object = args.user,
			owner = args.user,
			point = args.user.position,
			weapon = args.weapon}
	end)
end}
