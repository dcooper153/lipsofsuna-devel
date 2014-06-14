local ActionSpec = require("core/specs/action")
local Coroutine = require("system/coroutine")
local MissileSpell = require("core/objects/missilespell")

ActionSpec{name = "missile spell", func = function(feat, info, args)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
		feat:play_effects(args)
		local spec = Main.specs:find_by_name("SpellSpec", "magicmissile1")
		local spell = MissileSpell(args.user.manager, {feat = feat, owner = args.user, spec = spec})
		spell:fire{speed = 3}
	end)
end}
