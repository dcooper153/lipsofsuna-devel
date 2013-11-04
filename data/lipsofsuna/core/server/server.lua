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
local Database = require("system/database")
local DialogManager = require("core/dialog/dialog-manager")
local GlobalEventManager = require("core/server/global-event-manager")
local Lobby = require("system/lobby")
local Modifier = require("core/server/modifier")
local ModifierSpec = require("core/specs/modifier")
local Network = require("system/network")
local ObjectDatabase = require("core/server/object-database")
local Physics = require("system/physics")
local ServerConfig = require("core/server/server-config")
local Trading = require("core/server/trading")

--- TODO:doc
-- @type Server
Server = Class("Server")

Server.init = function(self, multiplayer, client)
	self.config = ServerConfig()
	self.marker_timer = 0
	self.initialized = true
	self.multiplayer = multiplayer
	self.client = client
	self.players_by_client = {}
	self.trading = Trading()
	-- Initialize the object database.
	self.object_database = ObjectDatabase(Main.game.database)
	if Main.settings.generate then
		self.object_database:reset()
	end
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
	self.config = nil
	self.players_by_client = nil
	self.trading = nil
	self.object_database = nil
	self.events = nil
	collectgarbage()
	-- Mark as uninitialized.
	self.initialized = false
	Physics:set_enable_simulation(false)
end

Server.authenticate_client = function(self, client, login, pass)
	-- Make sure not authenticated already.
	local account = Main.accounts:get_account_by_client(client)
	if account then return end
	-- Make sure not logging in twice.
	local account = Main.accounts:get_account_by_login(login)
	if account then
		if not Main.accounts:check_password(account, pass) then
			Main.log:format("Client login from %q failed: %s.", self:get_client_address(client), message)
			Main.messaging:server_event("login failed", client, "Invalid account name or password.")
			return
		end
		Main.log:format("Kicking client %q: %q logged into the same account.", self:get_client_address(account.client), self:get_client_address(client))
		self:kick_client(account.client)
	end
	-- Load or create an account.
	local account,message = Main.accounts:load_account(client, login, pass)
	if not account then
		if message then
			Main.log:format("Client login from %q failed: %s.", self:get_client_address(client), message)
			Main.messaging:server_event("login failed", client, "Invalid account name or password.")
			return
		end
		account = Main.accounts:create_account(client, login, pass)
	end
	-- Log the successful login.
	Main.log:format("Client login from %q using account %q.", self:get_client_address(client), login)
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
	-- Invoke login hooks.
	Main.accounts.login_hooks:call(account)
end

Server.kick_client = function(self, client)
	Network:disconnect(client)
	self:remove_client(client)
end

Server.remove_client = function(self, client)
	-- Detach the player object.
	local object = self.players_by_client[client]
	if object then
		self.object_database:save_object(object)
		object:detach()
		self.players_by_client[client] = nil
	end
	-- Update the account.
	Main.accounts:logout_client(client, object)
	-- Update lobby.
	Lobby:set_players(Lobby:get_players() - 1)
end

Server.send_game_state = function(self, player)
	-- Transmit the home marker.
	local client = player.client
	local home = player:get_spawn_point() or player:get_position():copy()
	Main.messaging:server_event("create marker", client, "home", home)
	-- Transmit unlocked map markers.
	for k,m in pairs(Main.markers.__dict_name) do
		if m.unlocked then
			Main.messaging:server_event("create marker", client, m.name, m.position)
		end
	end
	-- Transmit other unlocks.
	Main.messaging:server_event("unlocks init", client, Main.unlocks.unlocks)
	-- Transmit skills.
	player:update_skills()
	-- Transmit active and completed quests.
	if Main.quests then
		for k,q in pairs(Main.quests:get_all_quests()) do
			q:send_to_client(client, true, true)
		end
	end
	-- Transmit static objects.
	local objects = {}
	for k,v in pairs(Main.game.static_objects_by_id) do
		table.insert(objects, {v:get_id(), v.spec.name, v:get_position(), v:get_rotation()})
	end
	Main.messaging:server_event("create static objects", client, objects)
	-- Transmit dialog states of static objects.
	if Main.dialogs then
		for k,v in pairs(Main.dialogs.dialogs_by_object) do
			if v.object and v.object.static and v.event then
				player:vision_cb(v.event)
			end
		end
	end
end

Server.spawn_player = function(self, player, client, spawnpoint)
	-- Notify the client of the game start.
	Main.messaging:server_event("accept character", client)
	-- Add to the map.
	self.players_by_client[client] = player
	local home = player:get_spawn_point()
	if not home or spawnpoint then
		-- Teleport the player.
		home = player:set_spawn_point(spawnpoint)
		player:teleport{position = home}
		-- Spawn the player.
		player:add_modifier("respawn", 1)
	else
		-- Spawn the player.
		player:add_modifier("spawn", 1)
	end
	player:set_client(client)
	player:set_visible(true)
	-- Synchronize the state.
	self:send_game_state(player)
	-- Notify the global event manager.
	self.events:notify_action("player spawn", player)
end

Server.update = function(self, secs)
	if not self.initialized then return end
	-- Update markers.
	self.marker_timer = self.marker_timer + secs
	if self.marker_timer > 2 then
		self.marker_timer = 0
		for k,m in pairs(Main.markers.__dict_name) do
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
