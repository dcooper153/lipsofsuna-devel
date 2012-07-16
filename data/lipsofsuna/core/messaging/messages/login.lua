Message{
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
			Game.messaging:server_event("login failed", client, "Protocol mismatch.")
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
		Game.messaging:client_event("login", Settings.account, Settings.password)
	end}
