--- Area spell object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.areaspell
-- @alias AreaSpell

local Class = require("system/class")
local Damage = require("core/combat/damage")
local Spell = require("core/objects/spell")

--- Area spell object.
-- @type AreaSpell
local AreaSpell = Class("AreaSpell", Spell)

--- Creates a new area spell.
-- @param clss Area spell class.
-- @param manager Object manager.
-- @return AreaSpell.
AreaSpell.new = function(clss, manager)
	local self = Spell.new(clss, manager)
	self.timer = 1
	self.physics:set_collision_mask(0)
	self.physics:set_physics("static")
	return self
end

AreaSpell.contact_cb = function(self, args)
end

AreaSpell.update_client = function(self, secs)
	-- Play the starting effect.
	if self.started then return end
	self.started = true
	if not self.spec.effect then return end
	Main.effect_manager:play_object(self.spec.effect, self)
end

AreaSpell.update_server = function(self, secs)
	-- Update periodically.
	self.timer = self.timer + secs
	if self.timer < 0.5 then return end
	self.timer = self.timer - 0.5
	-- Apply the feat to each nearby object.
	local objs = Main.objects:find_by_point(self:get_position():copy():add_xyz(0,1,0), self.radius)
	for k,v in pairs(objs) do
		if v:get_visible() and v.class_name ~= "AreaSpell" then
			local damage = Damage()
			damage:add_spell_modifiers(self.modifiers)
			damage:apply_defender_vulnerabilities(v)
			Main.combat_utils:apply_damage_to_actor(self.owner, v, damage, v:get_position())
		end
	end
	-- Detach after timeout.
	self.duration = self.duration - 0.5
	if self.duration <= 0 then
		self:detach()
	end
end

return AreaSpell
