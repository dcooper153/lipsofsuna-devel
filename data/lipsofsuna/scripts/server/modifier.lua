Modifier = Class()
Modifier.types = {}

--- Creates a new modifier.
-- @param clss Modifier class.
-- @param args Arguments.<ul>
--   <li>args: Modifier specific arguments.</li>
--   <li>name: Modifier name. (required)</li>
--   <li>object: Object to modify. (required)</li></ul>
-- @return Modifier or nil.
Modifier.new = function(clss, object, name, args)
	if not args.object.modifiers then
		args.object.modifiers = {}
	end
	if args.object.modifiers[args.name] then
		return object.modifiers[args.name]
	elseif clss.types[args.name] then
		local self = Class.new(clss, {timer = 0, object = args.object})
		args.object.modifiers[args.name] = self
		self.routine = Thread(function()
			clss.types[args.name](self, args.args)
			object.modifiers[args.name] = nil
		end)
		return self
	end
	return nil
end

--- Registers a new modifier type.
-- @param clss Modifier class.
-- @param args Arguments.<ul>
--   <li>func: Handler function. (required)</li>
--   <li>name: Modifier name. (required)</li></ul>
Modifier.register = function(clss, args)
	clss.types[args.name] = args.func
end
