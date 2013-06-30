--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.obstacle
-- @alias Obstaclespec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type Obstaclespec
Obstaclespec = Spec:register("Obstaclespec", "obstacle", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "collision_group", type = "number", default = 0x1000, description = "Collision group."},
	{name = "collision_mask", type = "number", default = 0xFF, description = "Collision mask."},
	{name = "constraints", type = "list", list = {type = "string", details = {spec = "Constraintspec"}}, default = {}, description = "List of constraints."},
	{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
	{name = "harvest_behavior", type = "string", default = "keep", description = "Harvest behavior: keep/destroy."},
	{name = "harvest_effect", type = "string", description = "Effect to play when harvested."},
	{name = "harvest_materials", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of harvestable materials.", details = {keys = {spec = "Itemspec"}}},
	{name = "important", type = "boolean", default = true, description = "True to not allow the objectacle to be cleaned up from the map."},
	{name = "destroy_actions", type = "list", list = {type = "string"}, default = {}, description = "List of actions to perform when the obstacle is destroyed."},
	{name = "destroy_items", type = "list", list = {type = "spawn"}, default = {}, description = "List of items to spawn when the obstacle is destroyed."},
	{name = "health", type = "number", description = "Number of hit points the obstacle has."},
	{name = "interactive", type = "boolean", default = true, description = "False to make the object not appear interactive."},
	{name = "marker", type = "string", description = "Map marker name."},
	{name = "mass", type = "number", default = 10, description = "Mass in kilograms."},
	{name = "model", type = "string", description = "Model to use for the obstacle."},
	{name = "physics", type = "string", default = "static", description = "Physics mode."},
	{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "Effectspec"}}}, default = {}, description = "List of special effects to render."},
	{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "Actionspec"}}},
	{name = "vulnerabilities", type = "dict", dict = {type = "number"}, description = "Dictionary of damage vulnerabilities."}
})

--- Creates a new obstacle specification.
-- @param clss Obstaclespec class.
-- @param args Arguments.
-- @return New item specification.
Obstaclespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	self.harvest_enabled = args.harvest_materials ~= nil
	return self
end

--- Finds the constraints of the obstacle.
-- @param self Obstacle spec.
-- @return List of constraint specs, or nil.
Obstaclespec.get_constraints = function(self)
	if not self.constraints then return end
	local res = {}
	for k,v in pairs(self.constraints) do
		local con = Constraintspec:find{name = v}
		if con then
			table.insert(res, con)
		end
	end
	if #res == 0 then return end
	return res
end

--- Finds the special effects of the obstacle.
-- @param self Obstacle spec.
-- @return List of effect specs, or nil.
Obstaclespec.get_special_effects = function(self)
	if not self.special_effects then return end
	local res = {}
	for k,v in pairs(self.special_effects) do
		local eff = Effectspec:find{name = v}
		if eff then
			table.insert(res, eff)
		end
	end
	if #res == 0 then return end
	return res
end

--- Gets the use actions applicable to the obstacle.
-- @param self Obstacle spec.
-- @return List of actions specs.
Obstaclespec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Actionspec:find_by_name(k)
		if a then table.insert(res, a) end
	end
	return res
end

--- Returns a random obstacle spec.
-- @param clss Obstaclespec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li></ul>
-- @return Obstaclespec or nil.
Obstaclespec.random = function(clss, args)
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	if args and args.name then
		return clss.dict_name[args.name]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end


