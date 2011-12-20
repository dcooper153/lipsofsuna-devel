require "common/modelspec"

--- Finds or loads a model by name.
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>file: Model filename.</li>
--   <li>mesh: False to not load the mesh.</li></ul>
-- @return Model.
Model.find_or_load = function(clss, args)
	-- Reuse existing models.
	local self = clss:find(args)
	if self then return self end
	-- Load a new model.
	local spec = Modelspec:find{name = args.file}
	self = Model{name = spec and spec.file or args.file}
	self:load(args)
	-- Edit materials.
	if args.mesh ~= false and spec then
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
