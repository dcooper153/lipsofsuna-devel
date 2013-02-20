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
local Messaging = require("core/messaging/messaging")
local Network = require("system/network")
local ObjectManager = require("core/server/object-manager")
local Physics = require("system/physics")
local SectorManager = require("core/server/sector-manager")
local TerrainManager = require("core/server/terrain-manager")

--- TODO:doc
-- @type Game
Game = Class("Game")
Game.objects = ObjectManager() --FIXME
Game.scene_nodes_by_ref = {}

Game.PHYSICS_GROUP_ACTORS = 0x0001
Game.PHYSICS_GROUP_ITEMS = 0x0002
Game.PHYSICS_GROUP_PLAYERS = 0x0004
Game.PHYSICS_GROUP_TERRAIN = 0x0800
Game.PHYSICS_GROUP_OBSTACLES = 0x1000
Game.PHYSICS_GROUP_STATICS = 0x2000
Game.PHYSICS_GROUP_HEIGHTMAP = 0x4000
Game.PHYSICS_GROUP_VOXELS = 0x8000
Game.PHYSICS_MASK_CAMERA = 0xFF03
Game.PHYSICS_MASK_PICK = 0xFF03
Game.PHYSICS_MASK_TERRAIN = 0x00FF

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
	self.terrain = TerrainManager(8, 0.75, self.database, self.enable_unloading, self.enable_generation, self.enable_graphics)
	-- Initialize storage.
	self.static_objects_by_id = setmetatable({}, {__mode = "kv"})
	-- Initialize the server.
	if mode == "server" then
		Server:init(true, false)
		self.messaging = Messaging(port or Server.config.server_port)
	elseif mode == "host" then
		Server:init(true, true)
		self.messaging = Messaging(port or Server.config.server_port)
	elseif mode == "single" then
		Server:init(false, true)
		self.messaging = Messaging()
	else
		self.messaging = Messaging()
	end
	-- Initialize terrain updates.
	if Server.initialized then
		-- TODO: Stick terrain
	end
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
	self.objects:detach_all()
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
	self.messaging = nil
	collectgarbage()
end

return Game
