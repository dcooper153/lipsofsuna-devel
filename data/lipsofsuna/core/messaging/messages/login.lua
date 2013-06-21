-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Network = require("system/network")

Main.messaging:register_message{
	name = "login",
	client_to_server_encode = function(self, login, pass)
		return {"string", login, "string", pass}
	end,
	client_to_server_decode = function(self, packet)
		local ok,login,pass = packet:read("string", "string")
		if not ok then return {} end
		return {login, pass}
	end,
	client_to_server_handle = function(self, client, login, pass)
		if not login or not pass then
			Main.messaging:server_event("login failed", client, "Protocol mismatch.")
			Network:disconnect(client)
			return
		end
		Server:authenticate_client(client, login, pass)
	end,
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		Main.messaging:client_event("login", Settings.account, Settings.password)
	end}
