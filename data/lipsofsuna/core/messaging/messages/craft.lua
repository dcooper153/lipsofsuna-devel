-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Crafting = require("crafting/crafting")

Main.messaging:register_message{
	name = "craft",
	client_to_server_encode = function(self, id, name)
		return {"uint32", id, "string", name}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,name = packet:read("uint32", "string")
		if not ok then return end
		return {id, name}
	end,
	client_to_server_handle = function(self, client, id, name)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		if player:get_id() ~= id then return end
		local o = Crafting:craft(player, name, player.crafting_mode)
		if not o then return end
		player.inventory:merge_or_drop_object(o)
	end,
	server_to_client_encode = function(self, mode)
		return {"string", mode}
	end,
	server_to_client_decode = function(self, packet)
		local ok,mode = packet:read("string")
		if not ok then return end
		return {mode}
	end,
	server_to_client_handle = function(self, mode)
		-- FIXME: Hack.
		if mode == "spell" then
			Ui:set_state("enchant")
		else
			Operators.crafting:set_mode(mode)
			if mode ~= "default" or Ui:get_state() == "crafting" then
				Ui:set_state("crafting")
			end
		end
	end}
