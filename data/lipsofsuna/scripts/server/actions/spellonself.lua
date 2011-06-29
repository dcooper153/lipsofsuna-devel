-- Spell on self.
-- At the specific time into the attack animation, the effects of the
-- feat are applied to the attacker herself.
Actionspec{name = "spell on self", func = function(feat, info, args)
	Thread(function(t)
		feat:play_effects(args)
		Thread:sleep(args.user.spec.timing_spell_self * 0.02)
		feat:apply{
			attacker = args.user,
			charge = args.charge,
			point = args.user.position,
			target = args.user,
			weapon = args.weapon}
	end)
end}
