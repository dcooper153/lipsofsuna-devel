Message{
	name = "unlocks init",
	server_to_client_encode = function(self, unlocks)
		local data = {}
		for k,v in ipairs(unlocks) do
			table.insert(data, "string")
			table.insert(data, v[1])
			table.insert(data, "string")
			table.insert(data, v[2])
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local unlock = {}
		while true do
			local ok,t,n = packet:resume("string", "string")
			if not ok then break end
			if unlock[t] then
				unlock[t][n] = true
			else
				unlock[t] = {[n] = true}
			end
		end
		return {unlock}
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
				if not Server.initialized then
					Client.data.unlocks:lock(type, name)
				end
				if not Operators.play:is_startup_period() then
					Client:append_log("Locked " .. type .. ": " .. name)
				end
			end
		end
		-- Unlock items.
		for type,names in pairs(unlocks) do
			for name in pairs(names) do
				if not Client.data.unlocks:get(type, name) then
					if not Server.initialized then
						Client.data.unlocks:unlock(type, name)
					end
					if not Operators.play:is_startup_period() then
						Client:append_log("Unlocked " .. type .. ": " .. name)
					end
				end
			end
		end
	end}
