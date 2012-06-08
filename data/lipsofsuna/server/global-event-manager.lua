Globaleventmanager = Class()
Globaleventmanager.class_name = "Globaleventmanager"

--- Initializes the global event manager.
-- @param self Globaleventmanager.
Globaleventmanager.init = function(self)
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
end

--- Finds players near the given sector.
-- @param self Globaleventmanager.
-- @param id Sector ID.
-- @return Dictionary of players mapped to player states.
Globaleventmanager.find_players_exploring_sector = function(self, id)
	-- Find the closest player.
	local c = Generator.inst:get_sector_center_by_id(id)
	local nearest_dist
	local nearest_player
	for k,v in pairs(Player.clients) do
		local d = (v.position - c).length
		if not nearest_dist or d < nearest_dist then
			nearest_dist = d
			nearest_player = v
		end
	end
	-- Check that the distance is sane.
	local res = {}
	if nearest_dist > 100 then return res end
	nearest_dist = nearest_dist + 20
	-- Use the closest player as the reference to find other players.
	for k,v in pairs(Player.clients) do
		local d = (v.position - c).length
		if d < nearest_dist then
			res[v] = self.player_states[v]
		end
	end
	return res
end

--- Called when a player performs an action.
-- @param self Globaleventmanager.
-- @param action String describing the action.
-- @param player Player object, or nil.
Globaleventmanager.notify_action = function(self, action, player)
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
		local t = Program.time
		local n = #s.dialog
		if n == 0 then
			s.dialog[1] = t
		else
			if n >= 10 then s.dialog[10] = nil end
			table.insert(s.dialog, 1, t)
		end
	elseif action == "eat" or action == "drink" then
		-- Keep 10 last eating actions in memory.
		local t = Program.time
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
		local t = Program.time
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
-- @param self Globaleventmanager.
-- @param id Sector ID.
-- @param loaded True for loaded, false for newly created.
-- @param objects List of objects in the sector.
Globaleventmanager.sector_created = function(self, id, loaded, objects)
	-- Find the player who most likely triggered the creation.
	local c = Generator.inst:get_sector_center_by_id(id)
	local nearest_dist
	local nearest_player
	for k,v in pairs(Player.clients) do
		local d = (v.position - c).length
		if not nearest_dist or d < nearest_dist then
			nearest_dist = d
			nearest_player = v
		end
	end
	-- Take note of players who were close enough to qualify as finders.
	if nearest_dist < 100 then
		nearest_dist = nearest_dist + 20
		for k,v in pairs(Player.clients) do
			local d = (v.position - c).length
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
-- @param self Globaleventmanager.
-- @param name Event name
Globaleventmanager.start_event = function(self, name)
	-- Stop the potential old instance.
	self:stop_event(name)
	-- Start the new event.
	local event = self.events[name]
	if not event then return end
	event.start_time = Program.time
	event.spec:started(event)
	-- Log the event.
	Log:format("Started global event %q", name)
end

--- Stops an event.
-- @param self Globaleventmanager.
-- @param name Event name
Globaleventmanager.stop_event = function(self, name)
	-- Find the running event.
	local event = self.events[name]
	if not event then return end
	if not event.start_time then return end
	-- Mark the event as stopped.
	event.start_time = nil
	event.spec:stopped(event)
	-- Log the event.
	Log:format("Stopped global event %q", name)
end

--- Updates global events.
-- @param self Globaleventmanager.
-- @param secs Seconds since the last update.
Globaleventmanager.update = function(self, secs)
	-- Update peridically.
	self.timer = self.timer + secs
	if self.timer < 1 then return end
	self.timer = self.timer - 1
	-- Update running events.
	local now = Program.time
	for k,v in pairs(self.events) do
		if v.start_time and v.spec.duration and v.spec.duration < now - v.start_time then
			self:stop_event(k)
		end
		v.spec:update(v, 1)
	end
end
