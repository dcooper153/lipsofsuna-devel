--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.static
-- @alias StaticSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type StaticSpec
local StaticSpec = Spec:register("Staticspec", "static", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "collision_group", type = "number", default = 0x2000, description = "Collision group."},
	{name = "collision_mask", type = "number", default = 0xFF, description = "Collision mask."},
	{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "DialogSpec"}},
	{name = "marker", type = "string", description = "Map marker name."},
	{name = "model", type = "string", description = "Model to use for the obstacle."},
	{name = "position", type = "vector", default = Vector(), description = "Position vector."},
	{name = "rotation", type = "quaternion", default = Quaternion(), description = "Rotation quaternion."},
	{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "ActionSpec"}}}
})

--- Creates a new static map object specification.
-- @param clss StaticSpec class.
-- @param args Arguments.
-- @return New item specification.
StaticSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the use actions applicable to the static.
-- @param self Static spec.
-- @return List of actions specs.
StaticSpec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Main.specs:find_by_name("ActionSpec", k)
		if a then table.insert(res, a) end
	end
	return res
end

return StaticSpec
