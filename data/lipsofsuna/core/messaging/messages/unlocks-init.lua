-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "unlocks init",
	server_to_client_encode = function(self, unlocks)
		local data = {}
		for type,names in pairs(unlocks) do
			for name in pairs(names) do
				table.insert(data, "string")
				table.insert(data, type)
				table.insert(data, "string")
				table.insert(data, name)
			end
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local unlocks = {}
		while true do
			local ok,t,n = packet:resume("string", "string")
			if not ok then break end
			if unlocks[t] then
				unlocks[t][n] = true
			else
				unlocks[t] = {[n] = true}
			end
		end
		return {unlocks}
	end,
	server_to_client_handle = function(self, unlocks)
		-- Determine locked items.
		local lock = {}
		for type,names in pairs(Client.data.unlocks.unlocks) do
			for name in pairs(names) do
				if not unlocks[type] or not unlocks[type][name] then
					if lock[type] then
						lock[type][name] = true
					else
						lock[type] = {[name] = true}
					end
				end
			end
		end
		-- Lock items.
		for type,names in pairs(lock) do
			for name in pairs(names) do
				Client.data.unlocks:lock(type, name)
				if not Operators.play:is_startup_period() then
					Client:append_log("Locked " .. type .. ": " .. name)
				end
			end
		end
		-- Unlock items.
		for type,names in pairs(unlocks) do
			for name in pairs(names) do
				if not Client.data.unlocks:get(type, name) then
					Client.data.unlocks:unlock(type, name)
					if not Operators.play:is_startup_period() then
						Client:append_log("Unlocked " .. type .. ": " .. name)
					end
				end
			end
		end
	end}
