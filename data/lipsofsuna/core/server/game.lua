--- TODO:doc
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

--- TODO:doc
-- @type Game
Game = Class("Game")
Game.scene_nodes_by_ref = {}

--- Initializes the game.
-- @param self Game.
-- @param mode Game mode, "benchmark"/"editor"/"host"/"join"/"server".
-- @param save Save file name, or nil when not using local I/O.
-- @param port Server port number, or nil if not hosting.
Game.init = function(self, mode, save, port)
	if self.initialized then self:deinit() end
	self.initialized = true
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
end

--- Uninitializes the game.
-- @param self Game.
Game.deinit = function(self)
	if not self.initialized then return end
	self.initialized = nil
	self.mode = nil
	-- Deinitialized terrain updates.
	-- TODO: Stick terrain
	-- Terminate the server.
	if Server.initialized then
		Server.serialize:save()
		Server:deinit()
	end
	-- Detach all objects.
	self.sectors.database = nil
	Main.objects:detach_all()
	self.sectors:unload_all()
	self.static_objects_by_id = nil
	-- Detach scene nodes.
	for k in pairs(self.scene_nodes_by_ref) do
		k:detach()
		self.scene_nodes_by_ref[k] = nil
	end
	-- Shutdown networking.
	Network:shutdown()
	-- Garbage collect everything.
	self.database = nil
	self.sectors = nil
	collectgarbage()
end

return Game
