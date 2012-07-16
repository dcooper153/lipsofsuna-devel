require "system/model"

--- Finds or loads a model by name.
-- @param clss Model class.
-- @param file Model filename.
-- @return Model.
Model.find_or_load = function(clss, file)
	-- Reuse existing models.
	local self = clss:find{file = file}
	if self then return self end
	-- Load a new model.
	local spec = Modelspec:find{name = file}
	self = Model{name = file}
	self:load(file, Client and true or false)
	self:changed()
	-- Edit materials.
	if Client and spec then
		-- Replace shaders.
		if spec.replace_shaders then
			for k,v in pairs(spec.replace_shaders) do
				self:edit_material{match_shader = k, shader = v}
			end
		end
		-- Edit materials.
		if spec.edit_materials then
			for k,v in pairs(spec.edit_materials) do
				self:edit_material(v)
			end
		end
		-- Update the model.
		if spec.replace_shaders or spec.edit_materials then
			self:changed()
		end
	end
	return self
end

--- Updates the render and physics meshes of the model.
--
-- FIXME: Shouldn't overwrite the original.
--
-- @param self Model.
Model.changed = function(self)
	Los.model_changed(self.handle)
	if self.render then
		self.render:set_model(self)
	end
end

--- Gets the render model of the model.
-- @param self Model.
-- @return Render model.
Model.get_render = function(self)
	if not self.render then
		self.render = RenderModel(self)
	end
	return self.render
end
