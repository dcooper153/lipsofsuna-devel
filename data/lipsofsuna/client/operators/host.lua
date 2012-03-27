--- Switches the game to the hosting start state.
-- @param self Client class.
Client.host_game = function(self)
	-- Start the server thread.
	Program:unload_world()
	local opts = string.format("--file %s --server %s %d", Settings.file, Settings.address, Settings.port)
	if Settings.admin then opts = opts .. " -d" end
	if Settings.generate then opts = opts .. " -g" end
	self.threads.server = Thread("main.lua", opts)
	-- Set information for the UI.
	self.data.connection.mode = "host"
	self.data.connection.text = "Starting the server on " .. Settings.address .. ":" .. Settings.port .. "..."
	self.data.connection.active = true
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	-- Enter the start game mode.
	Ui.state = "start-game"
end

