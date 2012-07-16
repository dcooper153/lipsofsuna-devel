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
		local dialog = Dialog.dict_id[id]
		if not dialog or not dialog.choices or not player.vision.objects[dialog.object] then return end
		dialog:answer(player, text)
	end}
