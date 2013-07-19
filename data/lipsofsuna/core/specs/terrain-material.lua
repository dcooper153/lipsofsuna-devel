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
local Spec = require("core/specs/spec")

--- Specifies the behavior of terrain materials.
-- @type TerrainMaterial
local TerrainMaterialSpec = Spec:register("TerrainMaterialSpec", "terrain material", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "effect_build", type = "string", description = "Building effect.", details = {spec = "Effectspec"}},
	{name = "effect_collapse", type = "string", description = "Collapsing effect.", details = {spec = "Effectspec"}},
	{name = "mining_item", type = "string", description = "Item name obtained by mining.", details = {spec = "Itemspec"}},
	{name = "top_texture", type = "number", description = "Texture number of the top surface"},
	{name = "bottom_texture", type = "number", description = "Texture number of the bottom surface"},
	{name = "side_texture", type = "number", description = "Texture number of the side surfaces"},
	{name = "decoration_texture", type = "number", description = "Texture number of the decoration"}
})

--- Creates a new terrain material specification.
-- @param clss TerrainMaterialSpec class.
-- @param args Arguments.
-- @return TerrainMaterialSpec.
TerrainMaterialSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

return TerrainMaterialSpec
