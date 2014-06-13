--- Implements loading and caching of models.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.model_manager
-- @alias ModelManager

local Class = require("system/class")
local Model = require("system/model")

--- Implements loading and caching of models.
-- @type ModelManager
local ModelManager = Class("ModelManager")

--- Creates a new model manager.
-- @param clss ModelManager class.
-- @return ModelManager.
ModelManager.new = function(clss)
	local self = Class.new(clss)
	self.models_by_name = setmetatable({}, {__mode = "v"})
	self.seconds_by_model = {}
	self.update_timer = 0
	return self
end

--- Finds or loads a model by name.
-- @param self ModelManager.
-- @param name Model filename.
-- @return Model.
ModelManager.find_by_name = function(self, name)
	-- Find or load the model.
	local model = self.models_by_name[name]
	if not model then
		-- Load a new model.
		local spec = Main.specs:find_by_name("ModelSpec", name)
		model = Model()
		model.name = name
		model.loaded = model:load(spec and spec.model or name, Client and true or false)
		self.models_by_name[name] = model
		-- Edit materials.
		if Client and spec then
			-- Replace shaders.
			if spec.replace_shaders then
				for k,v in pairs(spec.replace_shaders) do
					model:edit_material{match_shader = k, shader = v}
				end
			end
			-- Edit materials.
			if spec.edit_materials then
				for k,v in pairs(spec.edit_materials) do
					model:edit_material(v)
				end
			end
		end
		-- Update the model.
		model:changed()
	end
	-- Update caching.
	self.seconds_by_model[model] = 10
	return model
end

--- Updates caching of models.
-- @param self ModelManager.
-- @param secs Seconds since the last update.
ModelManager.update = function(self, secs)
	-- Avoid updating too frequently.
	self.update_timer = self.update_timer + secs
	if self.update_timer < 3 then return end
	self.update_timer = 0
	-- Recreate the timing table.
	local res = {}
	for model,time in pairs(self.seconds_by_model) do
		if time > secs then
			res[model] = time - secs
		end
	end
	self.seconds_by_model = res
end

return ModelManager
