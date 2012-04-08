Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Ui.state = "play"
	Quickslots:reset()
end}

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Operators.chargen:init()
	Ui.state = "chargen"
end}

Protocol:add_handler{type = "AUTHENTICATE_REJECT", func = function(event)
	local ok,s = event.packet:read("string")
	if not ok then return end
	Ui.state = "start-game"
	Client.data.start_game.text = "Authentication failed: " .. s
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(event)
	Network:send{packet = Packet(packets.CLIENT_AUTHENTICATE, "string", Settings.account, "string", Settings.password)}
end}

Protocol:add_handler{type = "CREATE_STATIC_OBJECTS", func = function(event)
	while true do
		local ok,id,name,x,y,z,rx,ry,rz,rw = event.packet:resume(
			"uint32", "string",
			"float", "float", "float",
			"float", "float", "float", "float")
		if not ok then return end
		local spec = Staticspec:find{name = name}
		if spec then
			local pos = Vector(x,y,z)
			local rot = Quaternion(rx,ry,rz,rw)
			Staticobject{id = id, spec = spec, type = spec.type, model = spec.model,
				collision_group = spec.collision_group,
				collision_mask = spec.collision_mask,
				position = pos, rotation = rot, realized = true}
		end
	end
end}
