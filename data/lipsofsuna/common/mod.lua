--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module common.mod
-- @alias Mod

local Class = require("system/class")

--- TODO:doc
-- @type Mod
Mod = Class("Mod")

Mod.load = function(self, name)
	local prev_name = self.name
	local prev_path = self.path
	self.name = name
	self.path = name .. "/"
	require(name .. "/init")
	self.name = prev_name
	self.path = prev_path
end

Mod.load_optional = function(self, name)
	local prev_name = self.name
	local prev_path = self.path
	self.name = name
	self.path = name .. "/"
	pcall(require, name .. "/init")
	self.name = prev_name
	self.path = prev_path
end


