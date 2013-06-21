-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "update skills",
	client_to_server_encode = function(self, enabled)
		local data = {}
		for k in pairs(enabled) do
			table.insert(data, "string")
			table.insert(data, k)
		end
		return data
	end,
	client_to_server_decode = function(self, packet)
		local enabled = {}
		while true do
			local ok,s = packet:resume("string")
			if not ok then break end
			enabled[s] = true
		end
		return {enabled}
	end,
	client_to_server_handle = function(self, client, enabled)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Validate the input.
		for k,v in pairs(enabled) do
			local skill = Skillspec:find{name = k}
			if not skill or not Server.unlocks:get("skill", k) then
				enabled[k] = nil
			end
		end
		-- Enable and disable skills.
		player.skills:clear()
		for k in pairs(enabled) do
			player.skills:add_without_requirements(k)
		end
		player.skills:remove_invalid()
		-- Recalculate player attributes.
		player:update_skills()
	end,
	server_to_client_encode = function(self, enabled)
		local data = {}
		for k in pairs(enabled) do
			table.insert(data, "string")
			table.insert(data, k)
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local enabled = {}
		while true do
			local ok,s = packet:resume("string")
			if not ok then break end
			enabled[s] = true
		end
		return {enabled}
	end,
	server_to_client_handle = function(self, enabled)
		-- Enable and disable skills.
		Client.data.skills:clear()
		for name in pairs(enabled) do
			Client.data.skills:add(name)
		end
		-- Update the user interface.
		if Ui:get_state() == "skills" then
			Ui:restart_state()
		end
	end}

