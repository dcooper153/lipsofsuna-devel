--- Effect profile specification for actors.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.effect_profile
-- @alias EffectProfileSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Effect profile specification for actors.
-- @type EffectProfileSpec
local EffectProfileSpec = Spec:register("EffectProfileSpec", "effect profile", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "effects", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of effect specs.", details = {values = {spec = "Effectspec"}}},
	{name = "inherit", type = "list", list = {type = "string", details = {value = {spec = "EffectProfileSpec"}}}, default = {}, description = "List of inherited profiles."}
})

--- Creates a new effect profile spec.
-- @param clss EffectProfileSpec class.
-- @param args Arguments.
-- @return Effect profile spec.
EffectProfileSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the effect spec for the given effect in the profile.
-- @param self Effect profile spec.
-- @param name Effect name in the profile.
-- @return Effect spec, or nil.
EffectProfileSpec.get_effect = function(self, name)
	-- Try primary effects.
	local ename = self.effects[name]
	if ename then
		return Effectspec:find_by_name(ename)
	end
	-- Try inherited effect.
	for k,v in pairs(self.inherit) do
		local profile = self.dict_name[v]
		local effect = profile:get_effect(name)
		if effect then return effect end
	end
end

return EffectProfileSpec
