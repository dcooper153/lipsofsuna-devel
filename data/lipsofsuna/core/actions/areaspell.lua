local AreaSpell = require("core/objects/areaspell")
local Coroutine = require("system/coroutine")
local Feat = require("arena/feat")

Actionspec{
	name = "area spell",
	func = function(feat, info, args)
		Coroutine(function(t)
			Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
			feat:play_effects(args)
			-- Create a separate object for each effect.
			-- Area spells may have different durations so a combination of them
			-- cannot generally be represented as one object.
			for k,v in pairs(feat.effects) do
				local effect = Feateffectspec:find{name = v[1]}
				local spec = effect and Spellspec:find{name = effect.projectile}
				if effect and spec then
					local sub = Feat("area spell", {{v[1], v[2]}})
					local spell = AreaSpell{
						duration = effect.duration, radius = effect.radius,
						feat = sub, owner = args.user, position = args.user:get_position(),
						realized = true, spec = spec}
				end
			end
		end)
	end}
