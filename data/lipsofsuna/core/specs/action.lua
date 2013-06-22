--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.action
-- @alias Actionspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Actionspec
Actionspec = Class("Actionspec", Spec)
Actionspec.type = "action"
Actionspec.dict_id = {}
Actionspec.dict_cat = {}
Actionspec.dict_name = {}
Actionspec.introspect = Introspect{
	name = "Actionspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "func", type = "ignore"},
		{name = "label", type = "string", description = "Human readable name of the action."}
	}}

--- Creates a new action specification.
-- @param clss Actionspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the action belongs.</li>
--   <li>func: Function to handle the action.</li>
--   <li>name: Name of the action type.</li></ul>
-- @return New action specification.
Actionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

return Actionspec

