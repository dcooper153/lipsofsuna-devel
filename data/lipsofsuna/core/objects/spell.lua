--- Base class for spell objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.spell
-- @alias Spell

local Class = require("system/class")
local Combat = require("core/server/combat")
local Damage = require("arena/damage") --FIXME
local SimulationObject = require("core/objects/simulation")

--- Base class for spell objects.
-- @type Spell
local Spell = Class("Spell", SimulationObject)
Spell.dict_id = setmetatable({}, {__mode = "kv"})

--- Creates a new spell.
-- @param clss Spell class.
-- @param manager Object manager.
-- @return Spell.
Spell.new = function(clss, manager)
	local self = SimulationObject.new(clss, manager)
	self.dict_id[self:get_id()] = self
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Spell.contact_cb = function(self, result)
	-- Make sure that applicable.
	if not self:get_visible() then return end
	if result.object == self.owner then return end
	-- Apply the modifiers.
	if result.object then
		local damage = Damage()
		damage:add_spell_modifiers(self.modifiers)
		damage:apply_defender_vulnerabilities(result.object)
		Main.combat_utils:apply_damage_to_actor(self.owner, result.object, damage, result.point)
	else
		-- TODO: Move to CombatUtils.
		for k,v in pairs(self.modifiers) do
			if not Combat:apply_ranged_spell_impact(self.owner, self, k.name, result.point, nil, result.tile) then
				self.modifiers[k] = nil
			else
				more = true
			end
		end
	end
	-- Remove from the world.
	self:detach()
end

--- Fires the spell.
-- @param self Spell.
Spell.fire = function(self)
	local args = {collision = true, owner = self.owner}
	local ret = SimulationObject.fire(self, args)
	self.orig_rotation = self:get_rotation():copy()
	self.orig_velocity = self:get_velocity():copy()
	-- Play the effects.
	for k,v in pairs(self.modifiers) do
		if k.effect then
			Main.vision:object_effect(self, k.effect)
		end
	end
	return ret
end

--- Updates the spell.
-- @param self Spell.
-- @param secs Seconds since the last update.
Spell.update_server = function(self, secs)
	if not self:get_visible() then return end
	if not self:has_server_data() then return end
	-- Prevent impacts from altering movement.
	self:set_rotation(self.orig_rotation)
	self:set_velocity(self.orig_velocity)
end

--- Sets the modifiers of the spell.
-- @param self Spell.
-- @param value Dictionary of modifiers and their values.
Spell.set_modifiers = function(self, value)
	self.modifiers = value
end

--- Sets the owner of the spell.
-- @param self Spell.
-- @param value Object.
Spell.set_owner = function(self, value)
	self.owner = value
end

--- Sets the spec of the object.
-- @param self Actor.
-- @param value Spellspec.
Spell.set_spec = function(self, value)
	if self.spec == value then return end
	SimulationObject.set_spec(self, value)
	-- Set the model.
	self:set_model_name(value.model)
	-- Configure physics.
	self.physics:set_gravity(Vector())
	self.physics:set_physics("rigid")
end

return Spell
