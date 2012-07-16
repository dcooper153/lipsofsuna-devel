-- Spell on touch.
-- While the attack animation is played, an attack ray is cast.
-- The first object or tile that collides with the ray is damaged.
Actionspec{name = "spell on touch", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_spell_touch * 0.02)
		local src,dst = args.user:get_attack_ray()
		local r = Physics:cast_ray{src = src, dst = dst}
		if not r then return end
		feat:apply{
			charge = args.charge,
			object = r.object,
			owner = args.user,
			point = r.point,
			tile = r.tile,
			weapon = args.weapon}
	end)
end}
