local Coroutine = require("system/coroutine")

-- Ranged spell.
-- A magical projectile is fired at the specific time into the attack
-- animation. The collision callback of the projectile takes
-- care of damaging the hit object or tile.
Actionspec{name = "ranged spell", func = function(feat, info, args)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
		feat:play_effects(args)
		local spec = Spellspec:find{name = "fireball1"}
		local spell = Spell{feat = feat, owner = args.user, spec = spec}
		spell:fire()
	end)
end}
