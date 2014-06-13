--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.quest
-- @alias QuestSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type QuestSpec
local QuestSpec = Spec:register("QuestSpec", "quest", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "status", type = "string", default = "unused", description = "Quest status. (unused/inactive/active/completed)"},
	{name = "text", type = "string", default = "", description = "Textual description of the quest status."}
})

--- Creates a new quest specification.
-- @param clss QuestSpec class.
-- @param args Arguments.
-- @return QuestSpec.
QuestSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return QuestSpec
