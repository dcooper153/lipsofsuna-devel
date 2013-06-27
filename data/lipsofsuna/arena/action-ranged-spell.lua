local Feat = require("arena/feat")
local Spell = require("core/objects/spell")

Actionspec{
	name = "ranged spell",
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_ranged * 0.02
		return true
	end,
	update = function(action, secs)
		-- Apply the casting delay.
		action.object.cooldown = 1
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Play the effects.
		local feat = Feat("ranged spell", {{"fire damage", 10}}) --FIXME
		feat:play_effects{user = action.object}
		-- Fire the spell.
		local spec = Spellspec:find_by_name("fireball1") --FIXME
		if not spec then return end
		local spell = Spell(action.object.manager, {feat = feat, owner = action.object, spec = spec})
		spell:fire()
	end}
