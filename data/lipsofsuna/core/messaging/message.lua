--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.messaging.message
-- @alias Message

local Class = require("system/class")

--- TODO:doc
-- @type Message
Message = Class("Message")
Message.next_id = 1
Message.dict_id = {}
Message.dict_name = {}

Message.new = function(clss, args)
	-- Argument validation.
	assert(type(args.name) == "string")
	assert(args.name ~= "")
	assert(clss.dict_name[args.name] == nil)
	assert(args.client_to_server_handle or args.server_to_client_handle)
	if args.client_to_server_handle ~= nil then
		assert(type(args.client_to_server_handle) == "function")
		assert(type(args.client_to_server_encode) == "function")
		assert(type(args.client_to_server_decode) == "function")
	else
		assert(type(args.client_to_server_handle) == "nil")
		assert(type(args.client_to_server_encode) == "nil")
		assert(type(args.client_to_server_decode) == "nil")
		assert(type(args.client_to_server_predict) == "nil")
	end
	if args.server_to_client_handle ~= nil then
		assert(type(args.server_to_client_handle) == "function")
		assert(type(args.server_to_client_encode) == "function")
		assert(type(args.server_to_client_decode) == "function")
	else
		assert(type(args.server_to_client_handle) == "nil")
		assert(type(args.server_to_client_encode) == "nil")
		assert(type(args.server_to_client_decode) == "nil")
		assert(type(args.server_to_client_predict) == "nil")
	end
	-- Register the message type.
	local self = Class.new(clss)
	for k,v in pairs(args) do
		self[k] = v
	end
	self.id = clss.next_id
	clss.next_id = clss.next_id + 1
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	return self
end

return Message


