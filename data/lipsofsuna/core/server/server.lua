require "system/class"
require "common/crafting"
require "common/sectors"
require "common/unlocks"
require "server/util"
require "server/account"
require "server/serialize"
require "server/config"
require "server/dialog"
require "server/event"
require "server/global-event-manager"
require "server/log"
require "server/quest"
require "server/modifier"
require "server/particles"

Server = Class()
require "server/admin"

Server.init = function(self, multiplayer, client)
	self.config = Config()
	self.marker_timer = 0
	self.initialized = true
	self.multiplayer = multiplayer
	self.client = client
	self.accounts_by_client = {}
	self.accounts_by_name = setmetatable({}, {__mode = "v"})
	self.players_by_client = {}
	self.serialize = Serialize(Game.database, Game.sectors)
	-- Initialize the map generator.
	self.generator = Generator()
	-- Initialize the event manager.
	self.events = Globaleventmanager()
	-- Enable physics simulation.
	Physics.enable_simulation = true
end

Server.deinit = function(self)
	if not self.initialized then return end
	-- Garbage collect all data.
	--
	-- This needs to be done right now so that the databases are freed
	-- for sure. Otherwise, they might remain locked in memory when the
	-- client starts a new server.
	self.accounts_by_client = nil
	self.accounts_by_name = nil
	self.players_by_client = nil
	self.config = nil
	self.serialize = nil
	self.generator = nil
	self.events = nil
	collectgarbage()
	-- Mark as uninitialized.
	self.initialized = false
	Physics.enable_simulation = false
end

Server.load = function(self)
	-- Initialize the heightmap.
	if Map then Map:init() end
	-- Generate the map.
	if Settings.generate or self.serialize:get_value("map_version") ~= self.generator.map_version then
		self.generator:generate()
		self.serialize:set_value("data_version", self.serialize.data_version)
		Unlocks:init(self.serialize.db)
		Unlocks:write_db()
	else
		self.serialize:load()
		Unlocks:init(self.serialize.db)
		Unlocks:read_db()
	end
	-- Initialize default unlocks.
	Unlocks:unlock("skill", "Health lv1")
	Unlocks:unlock("skill", "Willpower lv1")
	Unlocks:unlock("spell type", "ranged spell")
	Unlocks:unlock("spell type", "spell on self")
	Unlocks:unlock("spell effect", "fire damage")
	Unlocks:unlock("spell effect", "light")
	Unlocks:unlock("spell effect", "physical damage")
	Unlocks:unlock("spell effect", "restore health")
end

Server.authenticate_client = function(self, client, login, pass)
	-- Make sure not authenticated already.
	local account = self.accounts_by_client[client]
	if account then return end
	-- Make sure not logging in twice.
	account = self.accounts_by_name[login]
	if account then
		Game.messaging:server_event("login failed", client, "The account is already in use.")
		return
	end
	-- Load or create the account.
	-- The password is also checked in case of an existing account. If the
	-- check fails, Account() returns nil and we disconnect the client.
	account = Account(login, pass)
	if not account then
		Game.messaging:server_event("login failed", client, "Invalid account name or password.")
		Network:disconnect(client)
		return
	end
	account.client = client
	self.accounts_by_client[client] = account
	-- Log the successful login.
	Log:format("Client login from %q using account %q.", self:get_client_address(client), login)
	-- Create existing characters.
	local object = Server.serialize:load_player_object(account)
	if object then
		object.account = account
		self:spawn_player(object, client)
	end
	-- Check for permissions.
	-- Check if the account has admin rights in the config file.
	-- Grant admin rights to the first client if started with --admin.
	local admin = (Server.config.admins[login] == true)
	if Settings.admin or client == -1 then
		Settings.admin = nil
		Server.config.admins[login] = true
		admin = true
	end
	-- Inform about admin privileges.
	Game.messaging:server_event("change privilege level", client, Server.config.admins[login] or false)
	-- Enter the character creation mode.
	if not object then
		Game.messaging:server_event("start character creation", client)
	end
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

Server.object_effect = function(self, object, name)
	if not name then return end
	Vision:event{type = "object-effect", object = object, effect = name}
end

Server.spawn_player = function(self, player, client, spawnpoint)
	-- Notify the client of the game start.
	Game.messaging:server_event("accept character", client)
	-- Add to the map.
	self.players_by_client[client] = player
	local home = player:get_spawn_point()
	if not home or spawnpoint then
		home = player:set_spawn_point(spawnpoint)
		player:teleport{position = home}
	end
	player:set_visible(true)
	player:set_client(client)
	-- Transmit the home marker.
	Game.messaging:server_event("create marker", client, "home", home)
	-- Transmit unlocked map markers.
	for k,m in pairs(Marker.dict_name) do
		if m.unlocked then
			Game.messaging:server_event("create marker", client, m.name, m.position)
		end
	end
	-- Transmit other unlocks.
	Game.messaging:server_event("unlocks init", client, Unlocks:get_list())
	-- Transmit skills.
	player:update_skills()
	-- Transmit active and completed quests.
	for k,q in pairs(Quest.dict_name) do
		q:send{client = client}
		q:send_marker{client = client}
	end
	-- Transmit static objects.
	local objects = {}
	for k,v in pairs(Game.static_objects_by_id) do
		table.insert(objects, {v.id, v.spec.name, v.position, v.rotation})
	end
	Game.messaging:server_event("create static objects", client, objects)
	-- Transmit dialog states of static objects.
	for k,v in pairs(Dialog.dict_id) do
		if v.object and v.object.static and v.event then
			player:vision_cb(v.event)
		end
	end
	-- Notify the global event manager.
	Server.events:notify_action("player spawn", player)
end

Server.update = function(self, secs)
	if not self.initialized then return end
	-- Update objects.
	for k,v in pairs(Object.objects) do
		v:update(secs)
	end
	-- Update markers.
	self.marker_timer = self.marker_timer + secs
	if self.marker_timer > 2 then
		self.marker_timer = 0
		for k,m in pairs(Marker.dict_name) do
			if m.unlocked and m.target then
				local o = SimulationObject:find{id = m.target}
				if o and (m.position - o.position).length > 1 then
					m.position = o.position
					for k,v in pairs(self.players_by_client) do
						Game.messaging:server_event("create marker", v, m.name, m.position)
					end
				end
			end
		end
	end
	-- Update global events.
	self.events:update(secs)
	-- Update world object decay.
	self.serialize:update_world_object_decay(secs)
end

Server.world_effect = function(self, point, name)
	if not name then return end
	Vision:event{type = "world-effect", point = point, effect = name}
end
