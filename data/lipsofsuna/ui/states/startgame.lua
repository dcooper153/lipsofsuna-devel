Ui:add_state{
	state = "start-game",
	root = "start-game",
	label = "Launching",
	update = function(secs)
		-- Check if already failed.
		if not Client.data.connection.active then return end
		-- Check if the server crashed when hosting.
		if Client.data.connection.mode == "host" then
			if Client.threads.server.done then
				Client:terminate_game()
				Client.data.connection.text = "The server terminated unexpectedly!"
				Client.data.connection.active = false
				Ui:restart_state()
				return
			end
		end
		-- If connecting, wait for the connection to establish.
		if Client.data.connection.connecting then
			if not Network.connected then return end
			Client.data.connection.text = "Waiting for reply from " .. Settings.address .. ":" .. Settings.port .. "..."
			Client.data.connection.connecting = false
			Client.data.connection.waiting = true
			Ui:restart_state()
		end
		-- If connected, wait for the login packet.
		if Client.data.connection.waiting then return end
		-- If hosting, wait for the server to initialize.
		if Client.data.connection.mode == "host" then
			local msg = Client.threads.server:pop_message()
			if not msg then return end
		end
		-- Connect to the server.
		if not Network:join{host = Settings.address, port = Settings.port} then
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
			else
				Client:join_game()
			end
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return Widgets.Uibutton("Cancel", function()
			Client:terminate_game()
			Ui.state = "mainmenu"
		end)
	end}

Ui:add_widget{
	state = "start-game",
	widget = function() return Widgets.Uilabel(Client.data.connection.text) end}
