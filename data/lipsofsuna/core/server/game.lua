--- Base game state.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.game
-- @alias Game

local Class = require("system/class")
local Database = require("system/database")
local Network = require("system/network")
local Physics = require("system/physics")
local SectorManager = require("core/server/sector-manager")

--- Base game state.
-- @type Game
local Game = Class("Game")

--- Creates a new game.
-- @param clss Game class.
-- @param mode Game mode, "benchmark"/"editor"/"host"/"join"/"server".
-- @param save Save file name, or nil when not using local I/O.
-- @param port Server port number, or nil if not hosting.
-- @return Game.
Game.new = function(clss, mode, save, port)
	local self = Class.new(clss)
	self.mode = mode
	-- Initialize settings.
	self.enable_graphics = (mode ~= "server")
	self.enable_prediction = (mode == "join")
	self.enable_unloading = (mode ~= "editor" and mode ~= "benchmark")
	self.enable_generation = (mode == "server" or mode == "host" or mode == "single")
	-- Initialize sectors.
	if save then
		self.database = Database("save" .. save .. ".sqlite")
		self.database:query("PRAGMA synchronous=OFF;")
		self.database:query("PRAGMA count_changes=OFF;")
	end
	self.sectors = SectorManager(self.database, self.enable_unloading)
	-- Initialize storage.
	self.static_objects_by_id = setmetatable({}, {__mode = "kv"})
	-- Call the game start hooks.
	Main.game = self --FIXME
	Main.game_start_hooks:call()
	return self
end

--- Frees the game.
-- @param self Game.
Game.free = function(self)
	-- Save the game.
	self:save()
	-- Terminate the server.
	if Server.initialized then
		Server:deinit()
	end
	-- Detach all objects.
	self.sectors.database = nil
	Main.objects:detach_all()
	self.sectors:unload_all()
	self.static_objects_by_id = nil
	-- Shutdown networking.
	Network:shutdown()
	-- Call the game end hooks.
	Main.game_end_hooks:call()
end

--- Loads the game from the database.
-- @param self Game.
Game.load = function(self)
	if not self.database then return end
	if Server.initialized then
		Server.serialize:load()
	end
	Main.game_load_hooks:call(self.database)
end

--- Saves the game into the database.
-- @param self Game.
-- @param reset True to reset tables. False otherwise.
Game.save = function(self, reset)
	if not self.database then return end
	if Server.initialized then
		Server.serialize:save(reset)
	end
	Main.game_save_hooks:call(self.database, reset)
end

return Game
