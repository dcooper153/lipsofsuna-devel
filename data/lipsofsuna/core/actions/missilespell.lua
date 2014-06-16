local ActionSpec = require("core/specs/action")
local Coroutine = require("system/coroutine")

ActionSpec{name = "missile spell", func = function(feat, info, args)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
		feat:play_effects(args)
		local spec = Main.specs:find_by_name("SpellSpec", "magicmissile1")
		local spell = args.user.manager:create_object_by_spell("MissilsSpell", "magicmissile1")
		if spell then
			spell.feat = feat
			spell.owner = args.owner
			spell:fire{speed = 3}
		end
	end)
end}
