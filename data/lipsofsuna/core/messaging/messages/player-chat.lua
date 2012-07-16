Message{
	name = "player chat",
	client_to_server_encode = function(self, text)
		return {"string", text}
	end,
	client_to_server_decode = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	client_to_server_handle = function(self, client, text)
		local player = Server:get_player_by_client(client)
		if not player then return end
		local cmd,match = ChatCommand:find(text, "server")
		if not cmd then return end
		if cmd.permission == "admin" and not player.admin then
			return player:send_message("You have no permission to do that.")
		end
		return cmd.func(player, {match})
	end}
