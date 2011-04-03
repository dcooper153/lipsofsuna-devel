require "system/class"

Model = Class()
Model.class_name = "Model"

--- Creates a new model.
-- @param clss Model class.
-- @param args Arguments.
-- @return New model.
Model.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.model_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Creates a copy of the model.
-- @param self Model.
-- @return New model.
Model.copy = function(self)
	local handle = Los.model_copy(self.handle)
	return Class.new(Model, {handle = handle})
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

Model.unittest = function()
	-- Creating models.
	local m = Model()
	assert(m)
	assert(m.handle)
	-- Copying models.
	local m1 = m:copy()
	assert(m1)
	assert(m1.handle)
	-- Function access.
	m:calculate_bounds()
	m:merge(m1)
end
