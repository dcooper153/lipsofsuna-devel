local Coroutine = require("system/coroutine")
local Spell = require("core/objects/spell")

Actionspec{
	name = "ranged spell",
	func = function(feat, info, args)
		Coroutine(function(t)
			Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
			feat:play_effects(args)
			local spec = Spellspec:find{name = "fireball1"}
			local spell = Spell(args.user.manager, {feat = feat, owner = args.user, spec = spec})
			spell:fire()
		end)
	end}
