ChatCommand = Class()
ChatCommand.class_name = "ChatCommand"
ChatCommand.dict_id = {}

--- Registers a new chat command.
-- @param clss Chat command class.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li>
--   <li>permission: Permission level.</ul>
-- @return New chat command.
ChatCommand.new = function(clss, args)
	local self = Class.new(clss, args)
	self.id = #clss.dict_id + 1
	clss.dict_id[self.id] = self
	return self
end

--- Finds a chat command.
-- @param clss Chat command class.
-- @param str Command string.
-- @return Chat command and its argument part, or nil.
ChatCommand.find = function(clss, str)
	for k,v in ipairs(clss.dict_id) do
		local match = string.match(str, v.pattern)
		if match then return v,match end
	end
end
