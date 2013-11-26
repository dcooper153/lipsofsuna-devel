-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "shortcut",
	client_to_server_encode = function(self, index, item, action)
		return {"uint8", index, "uint8", item or 0, "string", action or ""}
	end,
	client_to_server_decode = function(self, packet)
		local ok,index,item,action = packet:read("uint8", "uint8", "string")
		if not ok then return end
		return {index, item, action}
	end,
	client_to_server_handle = function(self, client, index, item, action)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if not player.account then return end
		-- Validate the parameters.
		if index < 1 or index > 10 then return end
		if action and #action > 32 then return end
		-- Save into the account.
		local del = (item == 0 or action == "" or action == nil)
		local s = player.account.shortcuts
		if s then
			if del then
				s[index] = nil
			else
				s[index] = {item,action}
			end
		elseif not del then
			player.account.shortcuts = {[index] = {item,action}}
		end
	end,
	server_to_client_encode = function(self, shortcuts)
		local data = {}
		for k,v in pairs(shortcuts) do
			table.insert(data, "uint8")
			table.insert(data, k)
			table.insert(data, "uint8")
			table.insert(data, v[1])
			table.insert(data, "string")
			table.insert(data, v[2])
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local shortcuts = {}
		while true do
			local ok,index,item,action = packet:resume("uint8", "uint8", "string")
			if not ok then break end
			shortcuts[index] = {item, action}
		end
		return shortcuts
	end,
	server_to_client_handle = function(self, shortcuts)
		Main.shortcuts:load_shortcuts(shortcuts)
	end}
