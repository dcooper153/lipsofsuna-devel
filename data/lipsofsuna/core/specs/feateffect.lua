--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.feateffect
-- @alias Feateffectspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Feateffectspec
Feateffectspec = Class("Feateffectspec", Spec)
Feateffectspec.type = "feateffect"
Feateffectspec.dict_id = {}
Feateffectspec.dict_cat = {}
Feateffectspec.dict_name = {}
Feateffectspec.introspect = Introspect{
	name = "Feateffectspec",
	fields = {
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
		{name = "influences", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of influences and their strengths.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "projectile", type = "string", description = "Spell object to use as a projectile.", details = {spec = "Spellspec"}},
		{name = "radius", type = "number", default = 0, description = "Area of effect radius."},
		{name = "range", type = "number", default = 0, description = "Maximum firing range for bullet and ray targeting modes."},
		{name = "required_reagents", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required reagents.", details = {keys = {spec = "Itemspec"}}},
		{name = "required_stats", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of required stats."},
		{name = "modifier", type = "ignore"},
		{name = "modifier_attributes", type = "ignore"},
		{name = "touch", type = "ignore"},
	}}

--- Registers a feat effect.
-- @param clss Feateffectspec class.
-- @param args Arguments.
-- @return New feat effect.
Feateffectspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

Feateffectspec.get_casting_info = function(self, result)
	local info = result or {}
	self.cooldown = self:get_cooldown(info.cooldown)
	self.stats = self:get_required_stats(info.stats)
	self.reagents = self:get_required_reagents(info.reagents)
	self.influences = self:get_influences(info.influences)
	return info
end

Feateffectspec.get_cooldown = function(self, result)
	return (cooldown or 0) + self.cooldown
end

Feateffectspec.get_influences = function(self, result)
	local influences = result or {}
	for name,value in pairs(self.influences) do
		influences[name] = (influences[name] or 0) + value
		if influences[name] == 0 then
			influences[name] = nil
		end
	end
	return influences
end

Feateffectspec.get_required_stats = function(self, result)
	local stats = result or {}
	for stat,value in pairs(self.required_stats) do
		stats[stat] = (stats[stat] or 0) + value
	end
	return res
end

Feateffectspec.get_required_reagents = function(self, result)
	local reagents = result or {}
	for reagent,value in pairs(self.required_reagents) do
		reagents[reagent] = (reagents[reagent] or 0) + value
	end
	return result
end


