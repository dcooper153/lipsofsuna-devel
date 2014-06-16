local ActionSpec = require("core/specs/action")
local Coroutine = require("system/coroutine")
local Feat = FIXME

ActionSpec{
	name = "area spell",
	func = function(feat, info, args)
		Coroutine(function(t)
			Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
			feat:play_effects(args)
			-- Create a separate object for each effect.
			-- Area spells may have different durations so a combination of them
			-- cannot generally be represented as one object.
			for k,v in pairs(feat.effects) do
				local effect = Main.specs:find_by_name("ModifierSpec", v[1])
				local spell = effect and args.user.manager:create_object_by_spec("AreaSpell", effect.projectile)
				if spell then
					local sub = Feat("area spell", {{v[1], v[2]}})
					spell.duration = effect.duration
					spell.radius = effect.radius
					spell.feat = sub
					spell.owner = args.user
					spell:set_position(args.user:get_position())
					spell:set_visible(true)
				end
			end
		end)
	end}
