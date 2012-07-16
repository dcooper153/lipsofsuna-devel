require "system/class"
require(Mod.path .. "message")

Messaging = Class()

Messaging.new = function(clss, port)
	local self = Class.new(clss)
	self:set_network_port(port)
	return self
end

Messaging.get_event_id = function(self, type)
	local handler = Message.dict_name[type]
	if not handler then return end
	return handler.id
end

Messaging.set_network_port = function(self, port)
	if port then
		self.multiplayer = true
		Network:host{port = port}
	else
		self.multiplayer = false
		Network:shutdown()
	end
end

Messaging.client_event = function(self, type, ...)
	-- Get the message handler.
	local handler = Message.dict_name[type]
	if not handler then return end
	assert(handler.client_to_server_encode)
	assert(handler.client_to_server_decode)
	assert(handler.client_to_server_handle)
	-- Handle the message.
	if Server.initialized then
		handler.client_to_server_handle(self, -1, ...)
	else
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

Messaging.server_event = function(self, type, client, ...)
	if not client then return end
	-- Get the message handler.
	local handler = Message.dict_name[type]
	if not handler then return end
	assert(handler.server_to_client_encode)
	assert(handler.server_to_client_decode)
	assert(handler.server_to_client_handle)
	-- Handle the message.
	if client == -1 then
		handler.server_to_client_handle(self, ...)
	else
		local args = handler.server_to_client_encode(self, ...)
		if not args then return end
		if args.class == Packet then
			Network:send{client = client, packet = args}
		else
			Network:send{client = client, packet = Packet(handler.id, unpack(args))}
		end
	end
end

Messaging.server_event_broadcast = function(self, type, ...)
	if Client then
		self:server_event(type, -1, ...)
	end
	if self.multiplayer then
		for k,v in pairs(Network.clients) do
			self:server_event(type, v, ...)
		end
	end
end

Messaging.handle_packet = function(self, client, packet)
	-- Get the message handler.
	local handler = Message.dict_id[packet.type]
	if not handler then return end
	-- Handle the message.
	if client then
		if not Server.initialized then return end
		local args = handler.client_to_server_decode(self, packet)
		if not args then
			print(string.format("WARNING: Failed to decode message %s from client", handler.name))
			return
		end
		handler.client_to_server_handle(self, client, unpack(args))
	else
		if not Client then return end
		local args = handler.server_to_client_decode(self, packet)
		if not args then
			print(string.format("WARNING: Failed to decode message %s from server", handler.name))
			return
		end
		handler.server_to_client_handle(self, unpack(args))
	end
end

------------------------------------------------------------------------------

Eventhandler{type = "packet", func = function(self, args)
	if Client then
		Game.messaging:handle_packet(args.client, args.packet)
	else
		Game.messaging:handle_packet(args.client, args.packet)
	end
end}
