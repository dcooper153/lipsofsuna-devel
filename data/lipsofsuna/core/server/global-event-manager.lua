--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.global_event_manager
-- @alias GlobalEventManager

local Actor = require("core/objects/actor")
local Class = require("system/class")
local Coroutine = require("system/coroutine")
local Sector = require("system/sector")

--- TODO:doc
-- @type GlobalEventManager
local GlobalEventManager = Class("GlobalEventManager")

--- Creates a new global event manager.
-- @param clss GlobalEventManager class.
-- @return GlobalEventManager.
GlobalEventManager.new = function(clss)
	local self = Class.new(clss)
	self.player_states = setmetatable({}, {__mode = "k"})
	self.timer = 0
	-- Create the events.
	self.events = {}
	for k,v in pairs(Globaleventspec.dict_name) do
		self.events[k] = {name = k, spec = v}
	end
	-- Start the initial events.
	self:start_event("brigand camp")
	self:start_event("random monsters")
	return self
end

--- Finds a random spawn point near an active player.
-- @param self Global event manager.
-- @return Position vector in world space, or nil.
GlobalEventManager.find_actor_spawn_point = function(self)
	-- Select a random player.
	local players = {}
	for client,player in pairs(Server.players_by_client) do
		table.insert(players, player)
	end
	if #players == 0 then return end
	local player = players[math.random(1, #players)]
	if not player.vision then return end
	-- Check that there are not too many actors nearby.
	local radius = player.vision:get_radius()
	local objects = Main.objects:find_by_point(player:get_position(), radius * 2)
	local monsters = 0
	for id,object in pairs(objects) do
		if object.class == Actor then
			monsters = monsters + 1
			if monsters > 3 then return end
		end
	end
	-- Select a random point just outside of the vision radius.
	local a = 2 * math.pi * math.random()
	local r = radius * 1.2
	local point = player:get_position():copy():add_xyz(r * math.cos(a), 0, r * math.sin(a))
	-- Check that no player sees the spawn point.
	for k,v in pairs(players) do
		if (point - v:get_position()).length < v.vision:get_radius() then
			return
		end
	end
	-- Find a valid spawn point near the chosen point.
	local spawn = Utils:find_spawn_point(point)
	if not spawn then return end
	-- Ensure that the point is in a loaded sector.
	local sector = Sector:get_id_by_point(spawn)
	-- FIXME: Why is the sector never loaded?
	--if not Game.sectors:is_sector_loaded(sector) then return end
	return spawn
end

--- Finds players near the given sector.
-- @param self GlobalEventManager.
-- @param id Sector ID.
-- @return Dictionary of players mapped to player states.
GlobalEventManager.find_players_exploring_sector = function(self, id)
	-- Find the closest player.
	local c = Sector:get_center_by_id(id)
	local nearest_dist
	local nearest_player
	for k,v in pairs(Server.players_by_client) do
		local d = (v:get_position() - c).length
		if not nearest_dist or d < nearest_dist then
			nearest_dist = d
			nearest_player = v
		end
	end
	-- Check that the distance is sane.
	local res = {}
	if not nearest_dist then return res end
	if nearest_dist > 100 then return res end
	nearest_dist = nearest_dist + 20
	-- Use the closest player as the reference to find other players.
	for k,v in pairs(Server.players_by_client) do
		local d = (v:get_position() - c).length
		if d < nearest_dist then
			res[v] = self.player_states[v]
		end
	end
	return res
end

--- Called when a player performs an action.
-- @param self GlobalEventManager.
-- @param action String describing the action.
-- @param player Player object, or nil.
GlobalEventManager.notify_action = function(self, action, player)
	-- We are only interested in player actions.
	if not player then return end
	if not player.client then return end
	-- Remove disabled players.
	if action == "player disable" then
		self.player_states[player] = nil
		return
	end
	-- Find or initialize the player state.
	local s = self.player_states[player]
	if not s then
		s = {dialog = {}, eaten = {}, explored = {}}
		self.player_states[player] = s
	end
	-- Handle the action.
	if action == "dialog" then
		-- Keep 10 last dialog actions in memory.
		local t = Program:get_time()
		local n = #s.dialog
		if n == 0 then
			s.dialog[1] = t
		else
			if n >= 10 then s.dialog[10] = nil end
			table.insert(s.dialog, 1, t)
		end
	elseif action == "eat" or action == "drink" then
		-- Keep 10 last eating actions in memory.
		local t = Program:get_time()
		local n = #s.eaten
		if n == 0 then
			s.eaten[1] = t
		elseif s.eaten[1] < t - 5 then
			if n >= 10 then s.eaten[10] = nil end
			table.insert(s.eaten, 1, t)
		end
	elseif action == "player explore" then
		-- Keep 10 last exploration actions in memory.
		--
		-- Since this action is based on sector loading, and that tends
		-- to occur in clusters, only one event per 5 seconds is recorded.
		local t = Program:get_time()
		local n = #s.explored
		if n == 0 then
			s.explored[1] = t
		elseif s.explored[1] < t - 5 then
			if n >= 10 then s.explored[10] = nil end
			table.insert(s.explored, 1, t)
		end
	end
end

--- Called when a sector is created or loaded.
-- @param self GlobalEventManager.
-- @param id Sector ID.
-- @param loaded True for loaded, false for newly created.
-- @param objects List of objects in the sector.
GlobalEventManager.sector_created = function(self, id, loaded, objects)
	-- Find the player who most likely triggered the creation.
	local c = Sector:get_center_by_id(id)
	local nearest_dist
	local nearest_player
	for k,v in pairs(Server.players_by_client) do
		local d = (v:get_position() - c).length
		if not nearest_dist or d < nearest_dist then
			nearest_dist = d
			nearest_player = v
		end
	end
	-- Take note of players who were close enough to qualify as finders.
	if nearest_dist and nearest_dist < 100 then
		nearest_dist = nearest_dist + 20
		for k,v in pairs(Server.players_by_client) do
			local d = (v:get_position() - c).length
			if d < nearest_dist then
				self:notify_action("player explore", v)
			end
		end
	end
	-- Spawn the sector.
	for k,v in pairs(self.events) do
		if v.start_time then
			Coroutine(function(thread)
				v.spec:sector_created(v, id, loaded, objects)
			end)
		end
	end
end

--- Starts an event.
-- @param self GlobalEventManager.
-- @param name Event name
GlobalEventManager.start_event = function(self, name)
	-- Stop the potential old instance.
	self:stop_event(name)
	-- Start the new event.
	local event = self.events[name]
	if not event then return end
	event.start_time = Program:get_time()
	event.spec:started(event)
	-- Log the event.
	Main.log:format("Started global event %q", name)
end

--- Stops an event.
-- @param self GlobalEventManager.
-- @param name Event name
GlobalEventManager.stop_event = function(self, name)
	-- Find the running event.
	local event = self.events[name]
	if not event then return end
	if not event.start_time then return end
	-- Mark the event as stopped.
	event.start_time = nil
	event.spec:stopped(event)
	-- Log the event.
	Main.log:format("Stopped global event %q", name)
end

--- Updates global events.
-- @param self GlobalEventManager.
-- @param secs Seconds since the last update.
GlobalEventManager.update = function(self, secs)
	-- Update peridically.
	self.timer = self.timer + secs
	if self.timer < 1 then return end
	self.timer = self.timer - 1
	-- Update running events.
	local now = Program:get_time()
	for k,v in pairs(self.events) do
		if v.start_time and v.spec.duration and v.spec.duration < now - v.start_time then
			self:stop_event(k)
		end
		v.spec:update(v, 1)
	end
end

return GlobalEventManager


