--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.faction
-- @alias Factionspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Factionspec
Factionspec = Class("Factionspec", Spec)
Factionspec.type = "faction"
Factionspec.dict_id = {}
Factionspec.dict_cat = {}
Factionspec.dict_name = {}
Factionspec.introspect = Introspect{
	name = "Factionspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "enemies", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of enemy faction names.", details = {keys = {spec = "Factionspec"}}}
	}}

--- Creates a new faction.
-- @param clss Factionspec class.
-- @param args Arguments.
-- @return Faction spec.
Factionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Adds an enemy faction.
-- @param self Faction spec.
-- @param args Arguments.<ul>
--   <li>name: Faction name.</li></ul>
Factionspec.add_enemy = function(self, args)
	self.enemies[args.name] = true
end


