Protocol:add_handler{type = "BOOK", func = function(event)
	local ok,title,text = event.packet:read("string", "string")
	if ok then
		Client.views.book:show{title = title, text = text}
		Client:set_mode("book")
	end
end}

Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Client:set_mode("game")
end}
Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Client:set_mode("chargen")
end}

Protocol:add_handler{type = "FEAT_UNLOCK", func = function(event)
	local ok,n = event.packet:read("string")
	if ok then
		local feat = Feat:find{name = n}
		if not feat then return end
		Gui.chat_history:append{text = "Unlocked feat " .. feat.name}
		feat.locked = false
	end
end}

Protocol:add_handler{type = "MESSAGE", func = function(event)
	local ok,msg = event.packet:read("string")
	if ok then
		Gui.chat_history:append{text = msg}
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

Protocol:add_handler{type = "AUTHENTICATE_REJECT", func = function(event)
	local ok,s = event.packet:read("string")
	if not ok then return end
	Client:set_mode("startup")
	Client.views.startup:set_state("Authentication failed: " .. s)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(event)
	Network:send{packet = Packet(packets.CLIENT_AUTHENTICATE, "string", Settings.account, "string", Settings.password)}
end}

Protocol:add_handler{type = "GENERATOR_STATUS", func = function(event)
	local ok,s,f = event.packet:read("string", "float")
	if ok then
		Client:set_mode("startup")
		Client.views.startup:set_state("Map generator: " .. s .. " (" .. math.ceil(f * 100) .. "%)")
	end
end}
