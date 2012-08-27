--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.lobby
-- @alias Lobby

local Class = require("system/class")

if not Los.program_load_extension("lobby") then
	error("loading extension `lobby' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Lobby
local Lobby = Class("Lobby")

--- Downloads the server list from the master server.
-- @param self Lobby class.
-- @return List of servers or nil if failed.
Lobby.download_server_list = function(self)
	if self.__master then
		return Los.lobby_download_server_list{master = self.__master}
	end
end

--- Sends server info to the master server.
-- @param self Lobby class.
Lobby.upload_server_info = function(self)
	if self.__desc and self.__master and self.__name and self.__players and self.__port then
		Los.lobby_upload_server_info{desc = self.__desc, master = self.__master,
			name = self.__name, players = self.__players, port = self.__port}
	end
end

Lobby.get_desc = function(self)
	return self.__desc
end

Lobby.set_desc = function(self, v)
	if self.desc == v then return end
	self.__desc = v
	self:upload_server_info()
end

Lobby.get_master = function(self)
	return self.__master
end

Lobby.set_master = function(self, v)
	if self.master == v then return end
	self.__master = v
	self:upload_server_info()
end

Lobby.get_name = function(self)
	return self.__name
end

Lobby.set_name = function(self, v)
	if self.name == v then return end
	self.__name = v
	self:upload_server_info()
end

Lobby.get_players = function(self)
	return self.__players
end

Lobby.set_players = function(self, v)
	if self.players == v then return end
	self.__players = v
	self:upload_server_info()
end

Lobby.get_port = function(self)
	return self.__port
end

Lobby.set_port = function(self, v)
	if self.port == v then return end
	self.__port = v
	self:upload_server_info()
end

return Lobby


