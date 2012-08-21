local Network = require("system/network")

Ui:add_state{
	state = "start-game",
	root = "start-game",
	label = "Launching",
	grab = function()
		return false
	end,
	update = function(secs)
		-- Check if already failed.
		if not Client.data.connection.active then return end
		-- If connecting, wait for the connection to establish.
		if Client.data.connection.connecting then
			if not Network:get_connected() then return end
			Client.data.connection.text = "Waiting for reply from " .. Settings.address .. ":" .. Settings.port .. "..."
			Client.data.connection.connecting = false
			Client.data.connection.waiting = true
			Ui:restart_state()
		end
		-- If connected, wait for the login packet.
		if Client.data.connection.waiting then return end
		-- Connect to the server.
		if Server.initialized then
			Client.data.connection.text = "Preparing the game..."
			Client.data.connection.connecting = true
			Ui:restart_state()
			Game.messaging:client_event("login", "local", "")
		elseif not Network:join{host = Settings.address, port = Settings.port} then
			Client.data.connection.text = "Failed to connect to " .. Settings.address .. ":" .. Settings.port .. "!"
			Client.data.connection.active = false
			Ui:restart_state()
		else
			Client.data.connection.text = "Connecting to " .. Settings.address .. ":" .. Settings.port .. "..."
			Client.data.connection.connecting = true
			Ui:restart_state()
		end
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return Widgets.Uibutton("Retry", function()
			Client:terminate_game()
			if Client.data.connection.mode == "host" then
				Client:host_game()
			elseif Client.data.connection.mode == "join" then
				Client:join_game()
			else
				Client:start_single_player()
			end
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return Widgets.Uibutton("Cancel", function()
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return Widgets.Uilabel(Client.data.connection.text) end}
