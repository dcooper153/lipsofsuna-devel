require "system/class"

if not Los.program_load_extension("render-model") then
	error("loading extension `render-model' failed")
end

------------------------------------------------------------------------------

RenderModel = Class()
RenderModel.class_name = "RenderModel"

--- Creates a new render model.
-- @param clss Render model class.
-- @param model Model.
-- @return New render model.
RenderModel.new = function(clss, model)
	local self = Class.new(clss)
	self.handle = Los.render_model_new(model.handle)
	return self
end

--- Returns true if the model has finished background loading.
-- @param self Render model.
-- @return True if finished loading, false if not.
RenderModel.get_loaded = function(self)
	return Los.render_model_get_loaded(self.handle)
end

--- Sets the model data used by the render model.
-- @param self Render model.
-- @param model Model.
RenderModel.set_model = function(self, model)
	Los.render_model_set_model(self.handle, model.handle)
end
