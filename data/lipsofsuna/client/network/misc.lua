Protocol:add_handler{type = "BOOK", func = function(event)
	local ok,title,text = event.packet:read("string", "string")
	if ok then
		Client.views.book:show{title = title, text = text}
		Client:set_mode("book")
	end
end}

Protocol:add_handler{type = "FEAT_UNLOCK", func = function(event)
	local ok,n = event.packet:read("string")
	if ok then
		local feat = Feat:find{name = n}
		if not feat then return end
		Client:append_log("Unlocked feat " .. feat.name)
		feat.locked = false
	end
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
	enabled = {}
	while true do
		local ok,name = event.packet:resume("string")
		if not ok then break end
		enabled[name] = true
	end
	-- Enable and disable skills.
	for k,v in pairs(Client.data.skills) do
		v.value = enabled[k] and true
	end
	-- Deactivate skills whose requirements aren't met.
	for k,v in pairs(Client.data.skills) do
		local skill = Skillspec:find{name = k}
		v.active = true
		for k1,name in pairs(skill.requires) do
			local req = Client.data.skills[name]
			if not req or not req.value then
				v.active = false
				break
			end
		end
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
		Client.data.start_game = {}
		Client.data.start_game.text = "Map generator: " .. s .. " (" .. math.ceil(f * 100) .. "%)"
		Client.data.start_game.active = true
		Client.data.start_game.waiting = true
		Ui.state = "start-game"
	end
end}
