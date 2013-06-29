local Feat = require("arena/feat")
local Spell = require("core/objects/spell")

Actionspec{
	name = "ranged spell",
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_ranged * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell ranged", true)
		return true
	end,
	update = function(action, secs)
		-- Apply the casting delay.
		action.object.cooldown = 1
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Get the influences.
		local names = {}
		if action.weapon.influences then
			for k,v in pairs(action.weapon.influences) do
				names[k] = (names[k] or 0) + v
			end
		else
			for k,v in pairs(action.weapon.spec.influences) do
				names[k] = (names[k] or 0) + v
			end
		end
		local influences = {}
		for k,v in pairs(names) do
			local spec = Feateffectspec:find_by_name(k)
			if spec then
				influences[k] = v
			end
		end
		-- Fire the spell.
		local spec = Spellspec:find_by_name("fireball1") --FIXME
		if not spec then return end
		local spell = Spell(action.object.manager)
		spell:set_spec(spec)
		spell:set_influences(influences)
		spell:set_owner(action.object)
		spell:fire()
	end}
