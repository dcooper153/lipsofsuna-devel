--- Implements communications between the server and the client.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module main.messaging
-- @alias Messaging

local Class = require("system/class")
local Message = require("main/message")
local Network = require("system/network")
local Packet = require("system/packet")

--- Implements communications between the server and the client.
-- @type Messaging
local Messaging = Class("Messaging")

--- Creates a new messaging system.
-- @param clss Messaging class.
-- @return Messaging.
Messaging.new = function(clss)
	local self = Class.new(clss)
	self.next_id = 1
	self.dict_id = {}
	self.dict_name = {}
	self.local_server = false
	self.local_client = false
	self.networking = false
	return self
end

--- Client sends a message to the server.
--
-- If the server is run in the same process, the message is passed to it
-- directly. Otherwise, it is encoded and sent over the network.
--
-- @param self Messaging.
-- @param type Message name.
-- @param ... Message arguments.
Messaging.client_event = function(self, type, ...)
	-- Get the message handler.
	local handler = self.dict_name[type]
	if not handler then return end
	assert(handler.client_to_server_encode)
	assert(handler.client_to_server_decode)
	assert(handler.client_to_server_handle)
	-- Handle the message.
	if self.local_server then
		handler.client_to_server_handle(self, -1, ...)
	else
		if not self.networking then
			error("tried to send remote event without connection to a remote server")
		end
		local args = handler.client_to_server_encode(self, ...)
		if not args then return end
		if handler.client_to_server_predict and not Server.initialized then
			handler.client_to_server_predict(self, ...)
		end
		if args.class == Packet then
			Network:send{packet = args}
		else
			Network:send{packet = Packet(handler.id, unpack(args))}
		end
	end
end

--- Registers a new message type.
-- @param self Messaging.
-- @param args Message arguments.
Messaging.register_message = function(self, args)
	-- Argument validation.
	assert(type(args.name) == "string")
	assert(args.name ~= "")
	assert(self.dict_name[args.name] == nil)
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
	-- Create the message.
	local msg = Message(self.next_id, args)
	self.next_id = self.next_id + 1
	self.dict_id[msg.id] = msg
	self.dict_name[msg.name] = msg
end

--- Server sends a message to an individual client.
--
-- If the client is run in the same process, the message is passed to it
-- directly. Otherwise, it is encoded and sent over the network.
--
-- @param self Messaging.
-- @param type Message name.
-- @param client Client ID.
-- @param ... Message arguments.
Messaging.server_event = function(self, type, client, ...)
	if not client then return end
	-- Get the message handler.
	local handler = self.dict_name[type]
	if not handler then return end
	assert(handler.server_to_client_encode)
	assert(handler.server_to_client_decode)
	assert(handler.server_to_client_handle)
	-- Handle the message.
	if client == -1 then
		if not self.local_client then
			error("tried to send local event without local client")
		end
		handler.server_to_client_handle(self, ...)
	else
		if not self.networking then
			error("tried to send remote event without hosting a networked server")
		end
		local args = handler.server_to_client_encode(self, ...)
		if not args then return end
		if args.class == Packet then
			Network:send{client = client, packet = args}
		else
			Network:send{client = client, packet = Packet(handler.id, unpack(args))}
		end
	end
end

--- Server sends a message to all clients.
-- @param self Messaging.
-- @param type Message name.
-- @param ... Message arguments.
Messaging.server_event_broadcast = function(self, type, ...)
	if self.local_client then
		self:server_event(type, -1, ...)
	end
	if self.networking then
		for k,v in pairs(Network:get_clients()) do
			self:server_event(type, v, ...)
		end
	end
end

--- Handles a received message.
-- @param self Messaging.
-- @param client Client ID. Nil if received by the client.
-- @param package Message packet.
Messaging.handle_packet = function(self, client, packet)
	-- Get the message handler.
	local handler = self.dict_id[packet:get_type()]
	if not handler then return end
	-- Handle the message.
	if client then
		if not self.local_server then
			error("received unexpected server packet")
		end
		local args = handler.client_to_server_decode(self, packet)
		if not args then
			print(string.format("WARNING: Failed to decode message %s from client", handler.name))
			return
		end
		handler.client_to_server_handle(self, client, unpack(args))
	else
		if not self.local_client then
			--error("received unexpected client packet")
			return
		end
		local args = handler.server_to_client_decode(self, packet)
		if not args then
			print(string.format("WARNING: Failed to decode message %s from server", handler.name))
			return
		end
		handler.server_to_client_handle(self, unpack(args))
	end
end

--- Gets the ID of the message of the given type.
-- @param self Messaging.
-- @param type Message name.
-- @return Message ID if found. Nil otherwise.
Messaging.get_event_id = function(self, type)
	local handler = self.dict_name[type]
	if not handler then return end
	return handler.id
end

--- Sets the transmission mode.
-- @param self Messaging.
-- @param local_server True if a local server is running. False otherwise.
-- @param local_client True if a local client is connected. False otherwise.
-- @param port Port number if remote clients may connect. Nil otherwise.
Messaging.set_transmit_mode = function(self, local_server, local_client, port)
	self.local_server = local_server
	self.local_client = local_client
	if port then
		self.networking = true
		Network:host{port = port}
	else
		self.networking = false
		Network:shutdown()
	end
end

return Messaging
