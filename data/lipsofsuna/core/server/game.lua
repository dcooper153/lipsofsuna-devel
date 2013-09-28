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
local OptionsDatabase = require("core/server/options-database")
local Physics = require("system/physics")
local SectorManager = require("core/server/sector-manager")

--- Base game state.
-- @type Game
local Game = Class("Game")

--- Creates a new game.
-- @param clss Game class.
-- @param mode Game mode.
-- @param save Save file name. Nil to not use local I/O.
-- @return Game.
Game.new = function(clss, mode, save)
	local self = Class.new(clss)
	self.mode = mode
	-- Initialize settings.
	self.enable_graphics = (mode ~= "server")
	self.enable_prediction = (mode == "join")
	self.enable_unloading = (mode ~= "editor" and mode ~= "benchmark")
	self.enable_generation = (mode == "server" or mode == "host" or mode == "single" or mode == "join")
	-- Initialize sectors.
	if save then
		self.database = Database("save" .. save .. ".sqlite")
		self.database:query("PRAGMA synchronous=OFF;")
		self.database:query("PRAGMA count_changes=OFF;")
	end
	self.sectors = SectorManager(12, self.database, self.enable_unloading)
	-- Initialize the options database.
	if save then
		self.options = OptionsDatabase(self.database)
	end
	-- Initialize storage.
	self.static_objects_by_id = setmetatable({}, {__mode = "kv"})
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
	Main.game_load_hooks:call(self.database)
end

--- Saves the game into the database.
-- @param self Game.
-- @param reset True to reset tables. False otherwise.
Game.save = function(self, reset)
	if not self.database then return end
	self.options:save(reset)
	Main.game_save_hooks:call(self.database, reset)
end

--- Starts the game.
-- @param self Game.
-- @return True if succeeded. False if the save file was invalid.
Game.start = function(self)
	-- Check for save version.
	if self.options and not self.options:check_version() then return end
	-- Call the game start hooks.
	Main.game_start_hooks:call()
	return true
end

return Game
