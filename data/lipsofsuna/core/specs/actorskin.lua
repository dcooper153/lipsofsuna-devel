--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.actorskin
-- @alias Actorskinspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Actorskinspec
Actorskinspec = Class("Actorskinspec", Spec)
Actorskinspec.type = "actorskinspec"
Actorskinspec.dict_id = {}
Actorskinspec.dict_cat = {}
Actorskinspec.dict_name = {}
Actorskinspec.introspect = Introspect{
	name = "Actorskinspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "actors", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of actor spec names.", details = {keys = {spec = "Actorspec"}}},
		{name = "material", type = "string", default = "animskin1", description = "Material name."},
		{name = "textures", type = "list", list = {type = "string"}, description = "List of texture names."}
	}}

--- Registers an actor skin.
-- @param clss Actorskinspec class.
-- @param args Arguments.
-- @return New actor skin spec.
Actorskinspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets skins by actor type.
-- @param self Actorskinspec class.
-- @param name Actor variant name.
-- @return List of skins.
Actorskinspec.find_by_actor = function(self, name)
	local res = {}
	for k,v in pairs(self.dict_name) do
		if v.actors[name] then table.insert(res, v) end
	end
	table.sort(res, function(a,b) return a.name < b.name end)
	return res
end


