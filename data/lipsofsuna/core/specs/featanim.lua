--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.featanim
-- @alias Feattypespec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Feattypespec
Feattypespec = Class("Feattypespec", Spec)
Feattypespec.type = "feattype"
Feattypespec.dict_id = {}
Feattypespec.dict_cat = {}
Feattypespec.dict_name = {}
Feattypespec.introspect = Introspect{
	name = "Feattypespec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "animation", type = "string", description = "Named of the animation played when the feat is performed."},
		{name = "action", type = "string", description = "The name of the action to perform."},
		{name = "bonuses_barehanded", type = "boolean", description = "True if bonuses from being bare-handed are added to damage."},
		{name = "bonuses_projectile", type = "boolean", description = "True if bonuses from the projectile are added to damage."},
		{name = "bonuses_weapon", type = "boolean", description = "True if bonuses from the weapon are added to damage."},
		{name = "cooldown", type = "number", default = 0, description = "Cooldown time in seconds."},
		{name = "description", type = "string", description = "Description of the feat."},
		{name = "directional", type = "boolean", description = "True to enable directional melee attacks."},
		{name = "effect", type = "string", description = "Sound effect played when the feat is performed."},
		{name = "effect_impact", type = "string", description = "Sound effect played when hits the target."},
		{name = "icon", type = "string", description = "Icon name.", default = "missing1", details = {spec = "Iconspec"}},
		{name = "influences", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of influences and their strengths.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "range", type = "number", description = "Maximum attack range estimate for the AI."},
		{name = "required_ammo", type = "string", description = "Name of the ammo item consumed by the feat, or \"WEAPON\" for using the ammo of the wielded weapon."},
		{name = "required_weapon", type = "string", description = "Weapon category required for performing the feat."},
		{name = "slot", type = "string", description = "Weapon slot used by the feat."},
		{name = "toggle", type = "boolean", description = "True to trigger the handler on the key release event as well."}
	}}

--- Registers a feat type.
-- @param clss Feattypespec class.
-- @param args Arguments.
-- @return New feat type.
Feattypespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	self.effects = {}
	return self
end

Feattypespec.get_casting_info = function(self, result)
	local info = result or {}
	info.cooldown = (info.cooldown or 0) + self.cooldown
	info.influences = self:get_influences(info.influences)
	return info
end

Feattypespec.get_influences = function(self, result)
	local influences = result or {}
	for name,value in pairs(self.influences) do
		influences[name] = (influences[n] or 0) + value
		if influences[name] == 0 then
			influences[name] = nil
		end
	end
	return influences
end


