--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module heightmap.init
-- @alias Map

local Class = require("system/class")
local Image = require("system/image")

Program:add_path(Mod.path)

--- TODO:doc
-- @type Map
Map = Class("Map")

Map.init = function(self)
	-- Only initialize once.
	if self.initialized then return end
	self.initialized = true
	-- Configuration.
	local position = Vector(1024, 1700, 1024)
	local size = 513
	local scaling = 80
	local spacing = 1
	-- Load the heightmap from a height image.
	local dimensions = Vector((size - 1) * spacing, 256 * scaling, (size - 1) * spacing)
	self.aabb = Aabb{point = position - dimensions * 0.5, size = dimensions}
end

Staticspec{
	name = "statictree1",
	model = "tree2"}


