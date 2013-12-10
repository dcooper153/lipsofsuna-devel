--- Hair style specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.hair_style
-- @alias HairStyleSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Hair style specification.
-- @type HairStyleSpec
local HairStyleSpec = Spec:register("HairStyleSpec", "hair style", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{
		name = "equipment_anchors",
		type = "dict",
		dict =
		{
			type = "list",
			list =
			{
				type = "struct",
				struct =
				{
					model = {type = "string"},
					model_color = {type = "string"},
					parent_node = {type = "string"},
					partition = {type = "string"},
					slot = {type = "string"},
					softbody_params = {type = "list", list = {type = "number"}},
					type = {type = "string"}
				}
			}
		},
		description = "Dictionary of equipment anchors."
	},
	{name = "equipment_priority", type = "number", default = 0, description = "The priority of equipment models and textures when worn."},
	{name = "equipment_textures", type = "dict", dict = {type = "dict", dict = {type = "string"}}, description = "Dictionary of equipment texture blit instructions. The dictionary keys are the base texture names and the values the source texture names."}
})

--- Creates a new hair style specification.
-- @param clss HairStyleSpec class.
-- @param args Arguments.
-- @return HairStyleSpec.
HairStyleSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Finds the equipment models of the haircut for the given race.
-- @param self HairStyleSpec.
-- @param name Name of the equipment class matching the race.
-- @param lod True for low level of detail.
-- @return Table of equipment models or nil.
HairStyleSpec.get_equipment_anchors = function(self, name, lod)
	-- Choose the level of detail.
	-- If the requested level doesn't exist, fall back to the other one.
	local anchors = nil
	if lod then anchors = self.equipment_anchors_lod end
	if not anchors then anchors = self.equipment_anchors end
	if not anchors then anchors = self.equipment_anchors_lod end
	-- Find the equipment anchors for the race.
	return anchors and anchors[name]
end

--- Finds the equipment textures of the item for the given race.
-- @param self HairStyleSpec.
-- @param name Name of the equipment class matching the race.
-- @param lod True for low level of detail.
-- @return Table of equipment textures or nil.
HairStyleSpec.get_equipment_textures = function(self, name, lod)
	-- Choose the level of detail.
	-- If the requested level doesn't exist, fall back to the other one.
	local textures = nil
	if lod then textures = self.equipment_textures_lod end
	if not textures then textures = self.equipment_textures end
	if not textures then textures = self.equipment_textures_lod end
	-- Find the equipment textures for the race.
	return textures and textures[name]
end

return HairStyleSpec
