Message{
	name = "server stats",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client)
		-- Check for permissions.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if not player.admin then
			Game.messaging:server_event("message", "You have no permission to do that.")
			return
		end
		-- Send the response.
		Game.messaging:server_event("server stats", Server:get_admin_stats())
	end,
	server_to_client = function(self, text)
		return {"string", text}
	end,
	server_to_client = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	server_to_client = function(self, text)
		Client.data.admin.server_stats = text
		Ui.state = "admin/server-stats"
	end}
