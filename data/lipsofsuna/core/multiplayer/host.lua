--- Hosts multiplayer games.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.multiplayer.host
-- @alias Host

local Class = require("system/class")
local Client = require("core/client/client")
local Sectors = require("system/sectors")
local Ui = require("ui/ui")

--- Hosts multiplayer games.
-- @type Host
local Host = Class("Host")

--- Creates a new multiplayer hoster.
-- @param clss Host class.
-- @return Host.
Host.new = function(clss)
	local self = Class.new(clss)
	self.data = {}
	self.data.file = "1"
	self.data.port = 10101
	return self
end

--- Starts hosting a multiplayer game.
-- @param self Host.
-- @param file Filename.
-- @param port Port.
Host.host_game = function(self, file, port)
	-- Initialize the state.
	self.data.file = file or self.data.file
	self.data.port = port or self.data.port
	self.data.state = "init"
	self.data.status = "Starting the server on " .. self.data.port .. "..."
	-- Start the server.
	Sectors:unload_all()
	Main.game = Game
	Main.game:init("host", self.data.file, self.data.port)
	Server:init(true, true)
	Main.messaging:set_transmit_mode(true, true, port or Server.config.server_port)
	Main:start_game("Host")
	-- Enter the waiting state.
	Ui:set_state("host/wait")
end

--- Updates the hosting status.
-- @param self Host.
-- @return True if the state changed. False otherwise.
Host.update = function(self, secs)
	if self.data.state == "init" then
		-- Send the login event.
		self.data.status = "Preparing the game..."
		self.data.state = "connecting"
		Main.messaging:client_event("login", "local", "")
		return true
	end
end

--- Gets the status message.
-- @param self Host.
-- @return String.
Host.get_status = function(self)
	return self.data.status
end

return Host
