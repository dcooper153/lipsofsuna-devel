--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.constraint
-- @alias Constraintspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Constraintspec
Constraintspec = Class("Constraintspec", Spec)
Constraintspec.type = "effect"
Constraintspec.dict_id = {}
Constraintspec.dict_cat = {}
Constraintspec.dict_name = {}
Constraintspec.introspect = Introspect{
	name = "Constraintspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "axis", type = "vector", description = "Axis of rotation for hinges."},
		{name = "constraint", type = "string", description = "Constraint type."},
		{name = "offset", type = "vector", description = "Position of the constraint relative to the parent object."}
	}}

--- Registers a new constraint spec.
-- @param clss Constraintspec class.
-- @param args Arguments.
-- @return New constraint spec.
Constraintspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end


