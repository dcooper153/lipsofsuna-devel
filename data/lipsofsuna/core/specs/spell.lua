--- Spell projectile specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.spell
-- @alias SpellSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Spell projectile specification.
-- @type SpellSpec
local SpellSpec = Spec:register("SpellSpec", "spell", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "effect", type = "string", description = "Effect to play when the spell is created.", details = {spec = "EffectSpec"}},
	{name = "model", type = "string", description = "Model to use for the spell."},
	{name = "particle", type = "string", description = "Particle effect to use for the spell."}
})

--- Creates a new spell specification.
-- @param clss SpellSpec class.
-- @param args Arguments.<ul>
--   <li>model: </li>
--   <li>name: </li>
--   <li>particle: </li></ul>
-- @return New item specification.
SpellSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return SpellSpec
