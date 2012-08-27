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

Physics.GROUP_ACTORS = 0x0001
Physics.GROUP_ITEMS = 0x0002
Physics.GROUP_PLAYERS = 0x0004
Physics.GROUP_OBSTACLES = 0x1000
Physics.GROUP_STATICS = 0x2000
Physics.GROUP_HEIGHTMAP = 0x4000
Physics.GROUP_VOXELS = 0x8000
Physics.MASK_CAMERA = 0xF003
Physics.MASK_PICK = 0xF003

--- TODO:doc
-- @type Game
Game = Class("Game")
Game.objects = ObjectManager() --FIXME
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
	-- Initialize sectors.
	if save then
		self.database = Database("save" .. save .. ".sqlite")
		self.database:query("PRAGMA synchronous=OFF;")
		self.database:query("PRAGMA count_changes=OFF;")
	end
	self.sectors = Sectors(self.database)
	-- Initialize settings.
	self.enable_graphics = (mode ~= "server")
	self.enable_prediction = (mode == "join")
	if mode == "editor" or mode == "benchmark" then
		self.sectors.unload_time = nil
	end
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
		Voxel.block_changed_cb = function(index, stamp)
			Server:global_event("voxel-block-changed", {index = index, stamp = stamp})
		end
	end
end

--- Uninitializes the game.
-- @param self Game.
Game.deinit = function(self)
	if not self.initialized then return end
	self.initialized = nil
	self.mode = nil
	-- Deinitialized terrain updates.
	Voxel.block_changed_cb = nil
	-- Terminate the server.
	if Server.initialized then
		Server.serialize:save()
		Server:deinit()
	end
	-- Detach all objects.
	self.sectors.database = nil
	self.objects:detach_all()
	self.sectors:unload_world()
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


