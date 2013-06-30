--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.help
-- @alias Helpspec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type Helpspec
Helpspec = Spec:register("Helpspec", "help", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "text", type = "string", description = "Help text."},
	{name = "title", type = "string", description = "Help page title."}
})

--- Registers a help specifification.
-- @param clss Helpspec class.
-- @param args Arguments.
-- @return New skill spec.
Helpspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end


