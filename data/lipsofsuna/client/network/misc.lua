Protocol:add_handler{type = "BOOK", func = function(event)
	local ok,title,text = event.packet:read("string", "string")
	if not ok then return end
	Client.data.book.title = title
	Client.data.book.text = text
	Ui.state = "book"
end}

Protocol:add_handler{type = "MESSAGE", func = function(event)
	local ok,msg = event.packet:read("string")
	if not ok then return end
	Client:append_log(msg)
end}

Protocol:add_handler{type = "MESSAGE_NOTIFICATION", func = function(event)
	local ok,msg = event.packet:read("string")
	if not ok then return end
	Client.notification_text = msg
end}

Protocol:add_handler{type = "PLAYER_SKILLS", func = function(event)
	-- Read the list of enabled skills.
	local enabled = {}
	while true do
		local ok,name = event.packet:resume("string")
		if not ok then break end
		enabled[name] = true
	end
	-- Enable and disable skills.
	Client.data.skills:clear()
	for name in pairs(enabled) do
		Client.data.skills:add(name)
	end
	-- Update the user interface.
	if Ui.state == "skills" then
		Ui:restart_state()
	end
end}

Protocol:add_handler{type = "VOXEL_DIFF", func = function(event)
	Voxel:set_block{packet = event.packet}
end}

Protocol:add_handler{type = "EFFECT_WORLD", func = function(event)
	local ok,t,x,y,z = event.packet:read("string", "float", "float", "float")
	if ok then
		Effect:play_world(t, Vector(x,y,z))
	end
end}

Protocol:add_handler{type = "GENERATOR_STATUS", func = function(event)
	local ok,s,f = event.packet:read("string", "float")
	if ok then
		-- TODO: Client:terminate_world()
		Client.data.connection = {}
		Client.data.connection.text = "Map generator: " .. s .. " (" .. math.ceil(f * 100) .. "%)"
		Client.data.connection.active = true
		Client.data.connection.waiting = true
		Ui.state = "start-game"
	end
end}

Protocol:add_handler{type = "UNLOCK", func = function(event)
	local lock = {}
	local unlock = {}
	-- Read unlocked items.
	while true do
		local ok,t,n = event.packet:resume("string", "string")
		if not ok then break end
		if unlock[t] then
			unlock[t][n] = true
		else
			unlock[t] = {[n] = true}
		end
	end
	-- Determine locked items.
	for type,names in pairs(Client.data.unlocks.unlocks) do
		for name in pairs(names) do
			if not unlock[type] or not unlock[type][name] then
				if lock[t] then
					lock[t][n] = true
				else
					lock[t] = {[n] = true}
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
	for type,names in pairs(unlock) do
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
