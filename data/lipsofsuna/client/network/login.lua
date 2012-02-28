Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Ui.state = "play"
	Quickslots:reset()
end}

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Client.views.chargen:init()
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
