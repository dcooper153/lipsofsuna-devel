--- Message handler.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module main.message
-- @alias Message

local Class = require("system/class")

--- Message handler.
-- @type Message
local Message = Class("Message")

--- Creates a new message.
-- @param id Message ID.
-- @param args Message arguments.
Message.new = function(clss, id, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do
		self[k] = v
	end
	self.id = id
	return self
end

return Message
