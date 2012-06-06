Globaleventmanager = Class()
Globaleventmanager.class_name = "Globaleventmanager"

--- Initializes the global event manager.
-- @param self Globaleventmanager.
Globaleventmanager.init = function(self)
	self.events = {}
	self.events_expired = {}
	self.timer = 0
	self:start_event("random monsters")
end

--- Called when a sector is created or loaded.
-- @param self Globaleventmanager.
-- @param id Sector ID.
-- @param loaded True for loaded, false for newly created.
-- @param objects List of objects in the sector.
Globaleventmanager.sector_created = function(self, id, loaded, objects)
	for k,v in pairs(self.events) do
		Coroutine(function(thread)
			v.spec:sector_created(v, id, loaded, objects)
		end)
	end
end

--- Starts an event.
-- @param self Globaleventmanager.
-- @param name Event name
Globaleventmanager.start_event = function(self, name)
	-- Find the event spec.
	local spec = Globaleventspec:find{name = name}
	if not spec then return end
	-- Stop the potential old instance.
	self:stop_event(name)
	-- Start the new event.
	local event = {name = name, spec = spec, start_time = Program.time}
	self.events[name] = event
	spec:started(event)
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
	-- Mark the event as stopped.
	self.events_expired[name] = nil
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
	-- Delete expired events.
	for k,v in pairs(self.events_expired) do
		if self.events[k] == v then
			self.events[k] = nil
		end
	end
	self.events_expired = {}
	-- Update running events.
	local now = Program.time
	for k,v in pairs(self.events) do
		if v.spec.duration and v.spec.duration < now - v.start_time then
			self:stop_event(k)
		else
			v.spec:update(v, 1)
		end
	end
end
