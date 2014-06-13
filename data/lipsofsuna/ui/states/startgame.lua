local Network = require("system/network")
local UiButton = require("ui/widgets/button")
local UiLabel = require("ui/widgets/label")

Ui:add_state{
	state = "start-game",
	root = "start-game",
	label = "Launching",
	update = function(secs)
		-- Check if already failed.
		if not Client.data.connection.active then return end
		-- Wait for the login packet.
		if Client.data.connection.connecting then return end
		-- Connect to the server.
		if Server.initialized then
			Client.data.connection.text = "Preparing the game..."
			Client.data.connection.connecting = true
			Ui:restart_state()
			Main.messaging:client_event("login", "local", "")
		end
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return UiButton("Retry", function()
			Client:terminate_game()
			Client:start_single_player()
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return UiButton("Cancel", function()
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return UiLabel(Client.data.connection.text) end}
