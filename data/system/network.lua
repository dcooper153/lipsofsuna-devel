if not Program:load_extension("network") then
	error("loading extension `network' failed")
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
