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
local Heightmap = require("system/heightmap")
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
	local heights = Image("terrainheight1.png")
	local dimensions = Vector((size - 1) * spacing, 256 * scaling, (size - 1) * spacing)
	self.aabb = Aabb{point = position - dimensions * 0.5, size = dimensions}
	self.heightmap = Heightmap{position = position, size = size, scaling = scaling,
		spacing = spacing, heights = heights}
	-- Setup rendering textures.
	if not Settings.server then
		self.heightmap:add_texture_layer{size = 10, name = "soil1",
			diffuse = "soil1", specular = "soil1s",
			normal = "soil1n", height = "soil1h"}
		self.heightmap:add_texture_layer{size = 10, name = "grass1",
			diffuse = "grass1", specular = "soil1s",
			normal = "grass1n", height = "soil1h", blend = "terrainsplat1"}
		self.heightmap:add_texture_layer{size = 30, name = "granite1",
			diffuse = "granite1", specular = "granite1s",
			normal = "granite1n", height = "soil1h", blend = "terrainsplat2"}
		self.heightmap:add_texture_layer{size = 20, name = "cobbles1",
			diffuse = "cobbles1", specular = "granite1s",
			normal = "cobbles1n", height = "soil1h", blend = "terrainsplat3"}
	end
end

Staticspec{
	name = "statictree1",
	model = "tree2"}


