require "common/modelspec"

local oldinst = Model.new
local oldload = Model.load
Model.models = {}
setmetatable(Model.models, {__mode = "v"})

--- Finds a model by name.
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>file: Model filename.</li></ul>
-- @return Model or nil.
Model.find = function(clss, args)
	if args.file then
		return clss.models[args.file]
	end
end

--- Finds or loads a model by name.
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>file: Model filename.</li>
--   <li>mesh: False to not load the mesh.</li></ul>
-- @return Model.
Model.load = function(clss, args)
	-- Reuse existing models.
	local self = clss:find(args)
	if self then return self end
	-- Load a new model.
	local spec = Modelspec:find{name = args.file}
	self = clss:new{name = spec and spec.file or args.file}
	oldload(self, args)
	-- Replace shaders.
	if args.mesh ~= false and spec and spec.replace_shaders then
		for k,v in pairs(spec.replace_shaders) do
			self:edit_material{match_shader = k, shader = v}
		end
		self:changed()
	end
	return self
end

--- Creates a new model.
-- @param clss Model class.
-- @param args Arguments.
-- @return New models.
Model.new = function(clss, args)
	local self = oldinst(clss, args)
	if self.name then
		clss.models[self.name] = self
	end
	return self
end
