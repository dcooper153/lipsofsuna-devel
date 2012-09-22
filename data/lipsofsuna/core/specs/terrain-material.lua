--- Specifies the behavior of terrain materials.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.terrain_material
-- @alias TerrainMaterialSpec

local Class = require("system/class")
require(Mod.path .. "spec")

--- Specifies the behavior of terrain materials.
-- @type TerrainMaterial
TerrainMaterialSpec = Class("TerrainMaterialSpec", Spec)
TerrainMaterialSpec.type = "terrain material"
TerrainMaterialSpec.dict_id = {}
TerrainMaterialSpec.dict_cat = {}
TerrainMaterialSpec.dict_name = {}
TerrainMaterialSpec.introspect = Introspect{
	name = "TerrainMaterialSpec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "effect_build", type = "string", description = "Building effect.", details = {spec = "Effectspec"}},
		{name = "effect_collapse", type = "string", description = "Collapsing effect.", details = {spec = "Effectspec"}},
		{name = "mining_item", type = "string", description = "Item name obtained by mining.", details = {spec = "Itemspec"}}
	}}

--- Creates a new terrain material specification.
-- @param clss TerrainMaterialSpec class.
-- @param args Arguments.
-- @return TerrainMaterialSpec.
TerrainMaterialSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end