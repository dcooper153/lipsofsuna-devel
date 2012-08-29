local Sectors = require("system/sectors")

--- Switches the game to the joining start state.
-- @param self Client class.
Client.join_game = function(self)
	-- Clear the world.
	Sectors:unload_all()
	Game:init("join")
	-- Set information for the UI.
	self.data.connection.mode = "join"
	self.data.connection.text = "Joining the server at " .. Settings.address .. ":" .. Settings.port .. "..."
	self.data.connection.active = true
	self.data.connection.connecting = false
	self.data.connection.waiting = false
	-- Enter the start game mode.
	Client.data.load.next_state = "start-game"
	Ui:set_state("load")
end

