Message{
	name = "choose dialog answer",
	client_to_server_encode = function(self, id, text)
		return {"uint32", id, "string", text}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,text = packet:read("uint32", "string")
		if not ok then return end
		return {id, text}
	end,
	client_to_server_handle = function(self, client, id, text)
		local player = Server:get_player_by_client(client)
		if not player then return end
		local object = Main.objects:find_by_id(id)
		if not object then return end
		if not player.vision.objects[object] then return end
		local dialog = Server.dialogs:find_by_object(object)
		if not dialog then return end
		if not dialog.choices then return end
		dialog:answer(player, text)
	end}
