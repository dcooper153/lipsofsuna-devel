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
local SimulationObject = require("core/objects/simulation")

--- Base class for spell objects.
-- @type Spell
local Spell = Class("Spell", SimulationObject)
Spell.dict_id = setmetatable({}, {__mode = "kv"})

--- Creates a new spell.
-- @param clss Spell class.
-- @param args Arguments.<ul>
--   <li>effect: Effect type.</li>
--   <li>feat: Feat invoking the spell.</li>
--   <li>model: Model name.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>position: Position in world space.</li>
--   <li>power: Effect power</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
Spell.new = function(clss, args)
	local self = SimulationObject.new(clss)
	self.dict_id[self:get_id()] = self
	self.effect = args.effect
	self.feat = args.feat
	self.owner = args.owner
	self.power = args.power
	self:set_spec(args.spec)
	self:set_gravity(Vector())
	self:set_model_name(args.spec.model)
	self:set_physics("rigid")
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Spell.contact_cb = function(self, result)
	-- Make sure that applicable.
	if not self:get_visible() then return end
	if result.object == self.owner then return end
	-- Call the collision callback of each effect.
	-- Effects can remove themselves from the feat by returning false.
	local left = 0
	for k,v in pairs(self.feat.effects) do
		local effect = Feateffectspec:find{name = v[1]}
		if not Combat:apply_ranged_spell_impact(self.owner, self, v[1], result.point, result.object, result.tile) then
			self.feat.effects[k] = nil
		else
			left = left + 1
		end
	end
	-- Detach if no effects were left.
	if left == 0 then
		self:detach()
	end
end

--- Fires the spell.
-- @param self Spell.
-- @param args Arguments.
Spell.fire = function(self, args)
	local a = args or {}
	a.collision = true
	a.feat = self.feat
	a.owner = self.owner
	local ret = SimulationObject.fire(self, a)
	self.orig_rotation = self:get_rotation():copy()
	self.orig_velocity = self:get_velocity():copy()
	return ret
end

--- Updates the spell.
-- @param self Spell.
-- @param secs Seconds since the last update.
Spell.update = function(self, secs)
	if not self:get_visible() then return end
	if self:has_server_data() then
		-- Prevent impacts from altering movement.
		self:set_rotation(self.orig_rotation)
		self:set_velocity(self.orig_velocity)
	end
	-- Update the base class.
	SimulationObject.update(self, secs)
end

--- Prevents spell from being saved.
-- @param self Spell.
Spell.write_db = function(self)
end

return Spell
