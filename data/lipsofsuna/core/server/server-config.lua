--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.server_config
-- @alias ServerConfig

local Class = require("system/class")
local ConfigFile = require("system/config-file")
local Lobby = require("system/lobby")

--- TODO:doc
-- @type ServerConfig
local ServerConfig = Class("ServerConfig")

--- Creates a new server configuration instance.
-- @param clss ServerConfig class.
-- @return Server config.
ServerConfig.new = function(clss)
	local self = Class.new(clss)
	-- Initialize defaults.
	self.admins = {}
	self.server_announce = false
	self.server_desc = "-"
	self.server_name = "Unnamed server"
	self.server_master = "http://lipsofsuna.org"
	self.server_port = 10101
	-- Load the configuration.
	self.config = ConfigFile{name = "server.cfg"}
	self:load()
	self:save()
	-- Setup the lobby.
	Lobby:set_players(0)
	if self.server_announce then
		Lobby:set_desc(self.server_desc)
		Lobby:set_name(self.server_name)
		Lobby:set_master(self.server_master)
		Lobby:set_port(self.server_port)
	end
	return self
end

--- Loads server configuration.
-- @param self Server config.
ServerConfig.load = function(self)
	local opts = {
		admins = function(v)
			self.admins = {}
			for w in string.gmatch(v, "[ \t\n]*([a-zA-Z0-9]+)[ \t\n]*") do
				self.admins[w] = true
			end
		end,
		server_announce = function(v) self.server_announce = (v ~= "false") end,
		server_desc = function(v) self.server_desc = v end,
		server_name = function(v) self.server_name = v end,
		server_master = function(v) self.server_master = v end,
		server_port = function(v) self.server_port = tonumber(v) end}
	-- Read values from the configuration file.
	for k in pairs(opts) do
		local v = self.config:get(k)
		if v then
			local opt = opts[k]
			opt(v)
		end
	end
end

--- Saves server configuration.
-- @param self Server config.
ServerConfig.save = function(self)
	-- Construct admin list.
	local a = {}
	for k in pairs(self.admins) do table.insert(a, k) end
	table.sort(a)
	local admins = ""
	for k,v in ipairs(a) do admins = admins .. v end
	-- Write server settings.
	self.config:set("admins", admins)
	self.config:set("server_announce", tostring(self.server_announce))
	self.config:set("server_desc", self.server_desc)
	self.config:set("server_name", self.server_name)
	self.config:set("server_master", self.server_master)
	self.config:set("server_port", tostring(self.server_port))
	-- Flush the file.
	self.config:save()
end

return ServerConfig


