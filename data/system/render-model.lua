--- Renderable model.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.render_model
-- @alias RenderModel

local Class = require("system/class")

if not Los.program_load_extension("render-model") then
	error("loading extension `render-model' failed")
end

------------------------------------------------------------------------------

--- Renderable model.
-- @type RenderModel
local RenderModel = Class("RenderModel")

--- Creates a new render model.
-- @param clss Render model class.
-- @param model Model.
-- @return New render model.
RenderModel.new = function(clss, model)
	local self = Class.new(clss)
	self.handle = Los.render_model_new(model.handle)
	return self
end

--- Replaces a texture with an image.
-- @param self Render model.
-- @param name Texture name without extension.
-- @param image Image.
RenderModel.replace_texture = function(self, name, image)
	Los.render_model_replace_texture(self.handle, name, image.handle)
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

return RenderModel
