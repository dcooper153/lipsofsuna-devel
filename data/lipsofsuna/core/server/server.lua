--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.server
-- @alias Server

local Class = require("system/class")
local AccountDatabase = require(Mod.path .. "account-database")
local Database = require("system/database")
local DialogManager = require("core/dialog/dialog-manager")
local GlobalEventManager = require(Mod.path .. "global-event-manager")
local Log = require(Mod.path .. "log")
local Marker = require("core/marker")
local Network = require("system/network")
local ObjectDatabase = require(Mod.path .. "object-database")
local Physics = require("system/physics")
local QuestDatabase = require("core/quest/quest-database")
local Serialize = require(Mod.path .. "serialize")
local ServerConfig = require(Mod.path .. "server-config")
local Trading = require(Mod.path .. "trading")
local UnlockManager = require(Mod.path .. "unlock-manager")
local Vision = require("system/vision")

--- TODO:doc
-- @type Server
Server = Class("Server")

Server.init = function(self, multiplayer, client)
	self.log = Log()
	self.config = ServerConfig()
	Main.dialogs = DialogManager()
	self.marker_timer = 0
	self.initialized = true
	self.multiplayer = multiplayer
	self.client = client
	self.accounts_by_client = {}
	self.accounts_by_name = setmetatable({}, {__mode = "v"})
	self.players_by_client = {}
	self.trading = Trading()
	-- Initialize the databases.
	local account_database = Database("accounts" .. Settings.file .. ".sqlite")
	account_database:query("PRAGMA synchronous=OFF;")
	account_database:query("PRAGMA count_changes=OFF;")
	self.serialize = Serialize(Game.database)
	self.unlocks = UnlockManager(Game.database)
	self.account_database = AccountDatabase(account_database)
	self.object_database = ObjectDatabase(Game.database)
	Main.quests = QuestDatabase(Game.database)
	if self.serialize:get_value("game_version") ~= self.serialize.game_version then
		self.unlocks:reset()
		Main.quests:reset()
		self.serialize:set_value("game_version", self.serialize.game_version)
	end
	if self.serialize:get_value("object_version") ~= self.serialize.object_version then
		self.object_database:reset()
		Server.serialize:set_value("object_version", self.serialize.object_version)
	end
	-- Initialize the map generator.
	self.generator = Generator()
	-- Initialize the event manager.
	self.events = GlobalEventManager()
	-- Enable physics simulation.
	Physics:set_enable_simulation(true)
end

Server.deinit = function(self)
	if not self.initialized then return end
	-- Garbage collect all data.
	--
	-- This needs to be done right now so that the databases are freed
	-- for sure. Otherwise, they might remain locked in memory when the
	-- client starts a new server.
	self.log = nil
	self.config = nil
	Main.dialogs = nil
	self.accounts_by_client = nil
	self.accounts_by_name = nil
	self.players_by_client = nil
	self.trading = nil
	self.config = nil
	self.serialize = nil
	self.unlocks = nil
	self.account_database = nil
	self.object_database = nil
	Main.quests = nil
	self.generator = nil
	self.events = nil
	collectgarbage()
	-- Mark as uninitialized.
	self.initialized = false
	Physics:set_enable_simulation(false)
end

Server.load = function(self)
	-- Initialize the heightmap.
	if Map then Map:init() end
	-- Generate the map.
	if Settings.generate or self.serialize:get_value("map_version") ~= self.generator.map_version then
		self.generator:generate()
		self.unlocks:save()
		self.serialize:set_value("data_version", self.serialize.data_version)
	else
		self.serialize:load()
		self.unlocks:load()
		Main.quests:load_quests()
		self.object_database:load_static_objects()
	end
	-- Initialize default unlocks.
	self.unlocks:unlock("skill", "Health lv1")
	self.unlocks:unlock("skill", "Willpower lv1")
	self.unlocks:unlock("spell type", "ranged spell")
	self.unlocks:unlock("spell type", "spell on self")
	self.unlocks:unlock("spell effect", "fire damage")
	self.unlocks:unlock("spell effect", "light")
	self.unlocks:unlock("spell effect", "physical damage")
	self.unlocks:unlock("spell effect", "restore health")
end

Server.authenticate_client = function(self, client, login, pass)
	-- Make sure not authenticated already.
	local account = self.accounts_by_client[client]
	if account then return end
	-- Make sure not logging in twice.
	account = self.accounts_by_name[login]
	if account then
		self.log:format("Client login from %q failed: account already in use.", self:get_client_address(client))
		Main.messaging:server_event("login failed", client, "The account is already in use.")
		return
	end
	-- Load or create an account.
	local account,message = self.account_database:load_account(login, pass)
	if not account then
		if message then
			self.log:format("Client login from %q failed: %s.", self:get_client_address(client), message)
			Main.messaging:server_event("login failed", client, "Invalid account name or password.")
			return
		end
		account = self.account_database:create_account(login, pass)
	end
	-- Associate the account to the client.
	account.client = client
	self.accounts_by_name[login] = account
	self.accounts_by_client[client] = account
	-- Log the successful login.
	self.log:format("Client login from %q using account %q.", self:get_client_address(client), login)
	-- Create existing characters.
	local object = self.object_database:load_player(account)
	if object then
		object.account = account
		self:spawn_player(object, client)
	end
	-- Check for permissions.
	-- Check if the account has admin rights in the config file.
	-- Grant admin rights to the first client if started with --admin.
	local admin = (self.config.admins[login] == true)
	if Settings.admin or client == -1 then
		Settings.admin = nil
		self.config.admins[login] = true
		admin = true
	end
	-- Inform about admin privileges.
	Main.messaging:server_event("change privilege level", client, self.config.admins[login] or false)
	-- Enter the character creation mode.
	if not object then
		Main.messaging:server_event("start character creation", client)
	end
end

Server.global_event = function(self, type, args)
	local a = args or {}
	a.type = type
	Vision:dispatch_event(a, Main.objects)
end

Server.object_effect = function(self, object, name)
	if not name then return end
	self:object_event(object, "object-effect", {effect = name})
end

Server.object_event = function(self, object, type, args)
	local a = args or {}
	a.id = object:get_id()
	a.object = object
	a.type = type
	Vision:dispatch_event(a, Main.objects)
end

Server.object_event_id = function(self, id, type, args)
	local a = args or {}
	a.id = id
	a.object = Main.objects:find_by_id(id)
	a.type = type
	Vision:dispatch_event(a, Main.objects)
end

Server.spawn_player = function(self, player, client, spawnpoint)
	-- Notify the client of the game start.
	Main.messaging:server_event("accept character", client)
	-- Add to the map.
	self.players_by_client[client] = player
	local home = player:get_spawn_point()
	if not home or spawnpoint then
		home = player:set_spawn_point(spawnpoint)
		player:teleport{position = home}
		player:inflict_modifier("respawn", 1)
	end
	player:set_client(client)
	player:set_visible(true)
	-- Transmit the home marker.
	Main.messaging:server_event("create marker", client, "home", home)
	-- Transmit unlocked map markers.
	for k,m in pairs(Marker.dict_name) do
		if m.unlocked then
			Main.messaging:server_event("create marker", client, m.name, m.position)
		end
	end
	-- Transmit other unlocks.
	Main.messaging:server_event("unlocks init", client, self.unlocks.unlocks)
	-- Transmit skills.
	player:update_skills()
	-- Transmit active and completed quests.
	for k,q in pairs(Main.quests:get_all_quests()) do
		q:send_to_client(client, true, true)
	end
	-- Transmit static objects.
	local objects = {}
	for k,v in pairs(Game.static_objects_by_id) do
		table.insert(objects, {v:get_id(), v.spec.name, v:get_position(), v:get_rotation()})
	end
	Main.messaging:server_event("create static objects", client, objects)
	-- Transmit dialog states of static objects.
	for k,v in pairs(Main.dialogs.dialogs_by_object) do
		if v.object and v.object.static and v.event then
			player:vision_cb(v.event)
		end
	end
	-- Notify the global event manager.
	self.events:notify_action("player spawn", player)
end

Server.update = function(self, secs)
	if not self.initialized then return end
	-- Update markers.
	self.marker_timer = self.marker_timer + secs
	if self.marker_timer > 2 then
		self.marker_timer = 0
		for k,m in pairs(Marker.dict_name) do
			if m.unlocked and m.target then
				local o = Main.objects:find_by_id(m.target)
				if o and (m.position - o:get_position()).length > 1 then
					m.position = o:get_position()
					for k,v in pairs(self.players_by_client) do
						Main.messaging:server_event("create marker", v, m.name, m.position)
					end
				end
			end
		end
	end
	-- Update global events.
	self.events:update(secs)
	-- Update world object decay.
	self.object_database:update_world_decay(secs)
end

Server.world_effect = function(self, point, name)
	if not name then return end
	local args = {type = "world-effect", point = point, effect = name}
	Vision:dispatch_event(args, Main.objects)
end

Server.get_client_address = function(self, client)
	if client == -1 then
		return "localhost"
	else
		return Network:get_client_address(client) or "???"
	end
end

Server.get_player_by_client = function(self, client)
	return self.players_by_client[client]
end


