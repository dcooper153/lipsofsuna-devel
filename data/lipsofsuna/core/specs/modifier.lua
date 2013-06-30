--- Spec for modifiers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.modifier
-- @alias ModifierSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Spec for modifiers.
-- @type ModifierSpec
local ModifierSpec = Class("ModifierSpec", Spec)
Spec:register("modifier", ModifierSpec, {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "actions", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of compatible actions.", details = {keys = {spec = "Actionspec"}}},
	{name = "affects_allies", type = "boolean", description = "True if the effect is applicable to allied actors."},
	{name = "affects_enemies", type = "boolean", description = "True if the effect is applicable to enemy actors."},
	{name = "affects_items", type = "boolean", description = "True if the effect is applicable to items."},
	{name = "affects_terrain", type = "boolean", description = "True if the effect is applicable to terrain."},
	{name = "cooldown", type = "number", default = 0, description = "Cooldown time in seconds."},
	{name = "description", type = "string", description = "Description of the feat effect."},
	{name = "duration", type = "number", default = 0, description = "Duration of the created spell object, in seconds."},
	{name = "effect", type = "string", description = "Named of the sound effect to play."},
	{name = "icon", type = "string", description = "Icon name.", default = "missing1", details = {spec = "Iconspec"}},
	{name = "influences", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of influences and their strengths.", details = {keys = {spec = "ModifierSpec"}}},
	{name = "projectile", type = "string", description = "Spell object to use as a projectile.", details = {spec = "Spellspec"}},
	{name = "radius", type = "number", default = 0, description = "Area of effect radius."},
	{name = "range", type = "number", default = 0, description = "Maximum firing range for bullet and ray targeting modes."},
	{name = "required_reagents", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required reagents.", details = {keys = {spec = "Itemspec"}}},
	{name = "required_stats", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required stats."},
	{name = "attributes", type = "ignore"},
	{name = "start", type = "ignore"},
	{name = "update", type = "ignore"},
})

--- Registers a feat effect.
-- @param clss ModifierSpec class.
-- @param args Arguments.
-- @return New feat effect.
ModifierSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

ModifierSpec.get_casting_info = function(self, result)
	local info = result or {}
	self.cooldown = self:get_cooldown(info.cooldown)
	self.stats = self:get_required_stats(info.stats)
	self.reagents = self:get_required_reagents(info.reagents)
	self.influences = self:get_influences(info.influences)
	return info
end

ModifierSpec.get_cooldown = function(self, result)
	return (cooldown or 0) + self.cooldown
end

ModifierSpec.get_influences = function(self, result)
	local influences = result or {}
	for name,value in pairs(self.influences) do
		influences[name] = (influences[name] or 0) + value
		if influences[name] == 0 then
			influences[name] = nil
		end
	end
	return influences
end

ModifierSpec.get_required_stats = function(self, result)
	local stats = result or {}
	for stat,value in pairs(self.required_stats) do
		stats[stat] = (stats[stat] or 0) + value
	end
	return res
end

ModifierSpec.get_required_reagents = function(self, result)
	local reagents = result or {}
	for reagent,value in pairs(self.required_reagents) do
		reagents[reagent] = (reagents[reagent] or 0) + value
	end
	return result
end

return ModifierSpec
