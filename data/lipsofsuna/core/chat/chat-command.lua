--- Chat command.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.chat.chat_command
-- @alias ChatCommand

local Class = require("system/class")

--- Chat command.
-- @type ChatCommand
local ChatCommand = Class("ChatCommand")

--- Registers a new chat command.
-- @param clss Chat command class.
-- @param id Command ID.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li>
--   <li>permission: Permission level.</ul>
-- @return New chat command.
ChatCommand.new = function(clss, id, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.id = id
	return self
end

return ChatCommand
