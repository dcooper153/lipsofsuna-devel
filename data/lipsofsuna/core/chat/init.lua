local Class = require("system/class")

ChatCommand = Class("ChatCommand")
ChatCommand.dict_id = {}

--- Registers a new chat command.
-- @param clss Chat command class.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li>
--   <li>permission: Permission level.</ul>
-- @return New chat command.
ChatCommand.new = function(clss, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.id = #clss.dict_id + 1
	clss.dict_id[self.id] = self
	return self
end

--- Finds a chat command.
-- @param clss Chat command class.
-- @param str Command string.
-- @param handler Handler name.
-- @return Chat command and its argument part, or nil.
ChatCommand.find = function(clss, str, handler)
	for k,v in ipairs(clss.dict_id) do
		if handler and (v.handler == handler or v.handler == "both") then
			local match = string.match(str, v.pattern)
			if match then return v,match end
		end
	end
end

require(Mod.path .. "commands")
