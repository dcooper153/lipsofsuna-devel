if not Program:load_extension("network") then
	error("loading extension `network' failed")
end

local network_getters = {
	clients = function(s) return s:get_clients() end,
	closed = function(s) return s:get_closed() end,
	connected = function(s) return s:get_connected() end}

local network_setters = {
	closed = function(s, v) s:set_closed(v) end}

Network.getter = function(self, key)
	local networkgetterfunc = network_getters[key]
	if networkgetterfunc then return networkgetterfunc(self) end
	return Class.getter(self, key)
end

Network.setter = function(self, key, value)
	local networksetterfunc = network_setters[key]
	if networksetterfunc then return networksetterfunc(self, value) end
	return Class.setter(self, key, value)
end

local packet_getters = {
	size = function(s) return s:get_size() end,
	type = function(s) return s:get_type() end}

local packet_setters = {
	type = function(s, v) s:set_type(v) end}

Packet.getter = function(self, key)
	local packetgetterfunc = packet_getters[key]
	if packetgetterfunc then return packetgetterfunc(self) end
	return Class.getter(self, key)
end

Packet.setter = function(self, key, value)
	local packetsetterfunc = packet_setters[key]
	if packetsetterfunc then return packetsetterfunc(self, value) end
	return Class.setter(self, key, value)
end
