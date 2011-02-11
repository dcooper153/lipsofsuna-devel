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
	local self = clss:find(args)
	if self then return self end
	self = clss:new{name = args.file}
	oldload(self, args)
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
