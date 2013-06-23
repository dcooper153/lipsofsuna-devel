-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
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
		local dialog = Main.dialogs:find_by_object(object)
		if not dialog then return end
		if not dialog.choices then return end
		dialog:answer(player, text)
	end}
