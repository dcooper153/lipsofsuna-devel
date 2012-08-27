--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.pattern
-- @alias Patternspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Patternspec
Patternspec = Class("Paternspec", Spec)
Patternspec.type = "pattern"
Patternspec.dict_id = {}
Patternspec.dict_cat = {}
Patternspec.dict_name = {}
Patternspec.introspect = Introspect{
	name = "Patternspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "size", type = "vector", default = Vector(4,4,4), decription = "Pattern size in tiles."},
		{name = "distance_pattern", type = "string", decription = "Position reference pattern.", details = {spec = "Patternspec"}},
		{name = "distance_min", type = "number", default = 11, decription = "Minimum distance to the reference pattern."},
		{name = "distance_max", type = "number", default = 51, decription = "Maximum distance to the reference pattern."},
		{name = "overworld", type = "boolean", default = false, decription = "True to allow generation in the overworld."},
		{name = "position", type = "vector", default = Vector(700,700,700), decription = "World map position, in tiles."},
		{name = "position_random", type = "vector", default = Vector(), decription = "Position randomization, in tiles."},
		{name = "spawn_point", type = "vector", decription = "Spawn point position, in tiles."},
		{name = "items", type = "map object list", default = {}, decription = "List of items to create."},
		{name = "obstacles", type = "map object list", default = {}, decription = "List of obstacles to create."},
		{name = "actors", type = "map object list", default = {}, decription = "List of actors to create."},
		{name = "statics", type = "map object list", default = {}, decription = "List of static objects to create."},
		{name = "tiles", type = "map tile list", default = {}, decription = "List of terrain tiles to create."}
	}}

--- Creates a new pattern specification.
-- @param clss Patternspec class.
-- @param args Arguments.
-- @return New pattern spec.
Patternspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Finds patterns that have a spawn point.
-- @param self Patternspec class.
-- @return Table of patterns.
Patternspec.find_spawn_points = function(self)
	local list = {}
	for k,v in pairs(self.dict_id) do
		if v.spawn_point then
			table.insert(list, v)
		end
	end
	return list
end

--- Writes the pattern to a string.
-- @param self Patternspec.
-- @return String.
Patternspec.write = function(self)
	return self.introspect:write_str(self)
end

Patternspec.get_spawn_point_world = function(self)
	if not self.spawn_point then return end
	return (self.position + self.spawn_point) * Voxel.tile_size
end


