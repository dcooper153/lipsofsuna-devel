--- Switches the game to the hosting start state.
-- @param self Client class.
Client.host_game = function(self)
	-- Start the server.
	Program:unload_world()
	Game:init("host", Settings.file, Settings.port)
	-- Set information for the UI.
	self.data.connection.mode = "host"
	self.data.connection.text = "Starting the server on " .. Settings.address .. ":" .. Settings.port .. "..."
	self.data.connection.active = true
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	-- Enter the start game mode.
	Client.data.load.next_state = "start-game"
	Ui:set_state("load")
end

Client.start_single_player = function(self)
	-- Start the server.
	Program:unload_world()
	Game:init("single", Settings.file)
	-- Set information for the UI.
	self.data.connection.mode = "single"
	self.data.connection.text = "Starting the server on " .. Settings.address .. ":" .. Settings.port .. "..."
	self.data.connection.active = true
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	-- Enter the start game mode.
	Client.data.load.next_state = "start-game"
	Ui:set_state("load")
end
