Effect = Class()
Effect.dict_name = {}

--- Finds an effect by name.
-- @param clss Effect class.
-- @param args Arguments.<ul>
--   <li>name: Effect name.</li></ul>
-- @return Effect or nil.
Effect.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Registers a new effect.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>model: Model effect name.</li>
--   <li>sound: Sound effect name.</li></ul>
-- @return New effect.
Effect.new = function(clss, args)
	local self = Class.new(clss, args)
	clss.dict_name[args.name] = self
	return self
end
