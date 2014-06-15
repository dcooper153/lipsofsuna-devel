--- Manages chat commands.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.chat.chat_manager
-- @alias ChatManager

local ChatCommand = require("core/chat/chat-command")
local Class = require("system/class")

--- Manages chat commands.
-- @type ChatManager
local ChatManager = Class("ChatManager")

--- Creates a new chat manager.
-- @param clss ChatManager class.
-- @return ChatManager.
ChatManager.new = function(clss, args)
	local self = Class.new(clss)
	self.__commands = {}
	return self
end

--- Finds a chat command.
-- @param self ChatManager.
-- @param str Command string.
-- @param handler Handler name.
-- @return ChatCommand and arguments on success. Nil otherwise.
ChatManager.find_command = function(self, str, handler)
	if not handler then return end
	for k,v in ipairs(self.__commands) do
		if not v.fallback and (v.handler == handler or v.handler == "both") then
			local match = string.match(str, v.pattern)
			if match then return v,match end
		end
	end
	for k,v in ipairs(self.__commands) do
		if v.fallback and (v.handler == handler or v.handler == "both") then
			local match = string.match(str, v.pattern)
			if match then return v,match end
		end
	end
end

--- Registers a new chat command.
-- @param clss ChatManager.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li>
--   <li>permission: Permission level.</ul>
-- @return ChatCommand.
ChatManager.register_command = function(self, args)
	local id = #self.__commands + 1
	local cmd = ChatCommand(id, args)
	self.__commands[id] = cmd
	return cmd
end

return ChatManager
