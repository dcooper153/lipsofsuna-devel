--- 3D model.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.model
-- @alias Model

local Aabb = require("system/math/aabb")
local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("model") then
	error("loading extension `model' failed")
end

------------------------------------------------------------------------------

--- 3D model.
-- @type Model
local Model = Class("Model")

--- Creates a new model.
-- @param clss Model class.
-- @param name Name for caching.
-- @return New model.
Model.new = function(clss, name)
	local self = Class.new(clss)
	self.handle = Los.model_new()
	return self
end

--- Creates a new model from an internal handle.
-- @param clss Model class.
-- @param handle Handle.
-- @return New model.
Model.new_from_handle = function(clss, handle)
	local self = Class.new(clss)
	self.handle = handle
	return self
end

--- Creates a copy of the model.
-- @param self Model.
-- @param shape_keys False to not copy shape keys.
-- @return New model.
Model.copy = function(self, shape_keys)
	local copy = Class.new(Model)
	copy.handle = Los.model_copy(self.handle, shape_keys)
	if self.name then
		copy.name = "*" .. self.name
	end
	return copy
end

--- Recalculates the bounding box of the model.
-- @param self Model.
Model.calculate_bounds = function(self)
	Los.model_calculate_bounds(self.handle)
end

--- Updates the render and physics meshes of the model.
-- @param self Model.
Model.changed = function(self)
	Los.model_changed(self.handle)
	if self.render then
		self.render:set_model(self)
	end
end

--- Loads the model from a file.
-- @param self Model.
-- @param ... Arguments.<ul>
--   <li>1,file: Filename.</li>
--   <li>2,mesh: False to not load the mesh.</li></ul>
-- @return True if loaded successfully.
Model.load = function(self, ...)
	local a,b = ...
	if type(a) == "table" then
		return Los.model_load(self.handle, a.file, a.mesh)
	else
		return Los.model_load(self.handle, a, b)
	end
end

--- Adds an additional model mesh to the model.
-- @param self Model.
-- @param args Arguments.<ul>
--   <li>1,model: Model.</li></ul>
Model.merge = function(self, args)
	if args.class then
		Los.model_merge(self.handle, args.handle)
	else
		Los.model_merge(self.handle, args.model.handle)
	end
end

--- Gets the local bounding box of the model.
-- @param self Model.
-- @return Aabb.
Model.get_bounding_box = function(self)
	local h1,h2 = Los.model_get_bounding_box(self.handle)
	local min = Vector:new_from_handle(h1)
	local max = Vector:new_from_handle(h2)
	return Aabb{point = min, size = max - min}
end

--- Gets the local center offset of the bounding box of the model.
-- @param self Model.
-- @return Vector.
Model.get_center_offset = function(self)
	local h = Los.model_get_center_offset(self.handle)
	return Vector:new_from_handle(h)
end

--- Gets the approximate memory used by the model, in bytes.
-- @param self Model.
-- @return Memory used, in bytes
Model.get_memory_used = function(self)
	return Los.model_get_memory_used(self.handle)
end

--- Gets the render model of the model.
-- @param self Model.
-- @return Render model.
Model.get_render = function(self)
	if not self.render then
		local RenderModel = require("system/render-model")
		self.render = RenderModel(self)
	end
	return self.render
end

--- Gets the vertex count of the model.
-- @param self Model.
-- @return Number.
Model.get_vertex_count = function(self)
	return Los.model_get_vertex_count(self.handle)
end

--- Gets the approximate total memory used by all models, in bytes.
-- @param clss Model class.
-- @return Memory used, in bytes
Model.get_total_model_count = function(clss)
	return Los.model_get_total_model_count()
end

--- Gets the approximate total memory used by all models, in bytes.
-- @param clss Model class.
-- @return Memory used, in bytes
Model.get_total_memory_used = function(self)
	return Los.model_get_total_memory_used()
end

return Model
