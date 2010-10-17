local instfunc = Model.new
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
--   <li>file: Model filename.</li></ul>
-- @return Model.
Model.load = function(clss, args)
	return clss:find(args) or clss:new(args)
end

--- Creates a new model.
-- @param clss Model class.
-- @param args Arguments.
-- @return New models.
Model.new = function(clss, args)
	local self = instfunc(clss, args)
	if args and args.file then
		clss.models[args.file] = self
		self.name = args.file
	end
	return self
end
