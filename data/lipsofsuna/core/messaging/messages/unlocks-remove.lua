-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "unlocks remove",
	server_to_client_encode = function(self, type, name)
		return {"string", type, "string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,type,name = packet:read("string", "string")
		if not ok then return end
		return {type, name}
	end,
	server_to_client_handle = function(self, type, name)
		Client.data.unlocks:lock(type, name)
		if not Operators.play:is_startup_period() then
			Client:append_log("Locked " .. type .. ": " .. name)
		end
	end}
