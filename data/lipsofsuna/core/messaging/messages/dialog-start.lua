Message{
	name = "dialog start",
	client_to_server_encode = function(self, id)
		return {"uint32", id}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	client_to_server_handle = function(self, client, id)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Find the object.
		local object = Game.objects:find_by_id(id)
		if not object then return end
		if not player:can_reach_object(object) then return end
		-- Execute the dialog of the object.
		if Server.dialogs:execute(object, player) then
			Game.messaging:server_event("object dialog", player.client, id)
		end
	end}
