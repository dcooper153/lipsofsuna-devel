local program_getters = {
	args = function(s) return s:get_args() end,
	quit = function(s) return s:get_quit() end,
	sectors = function(s) return s:get_sectors() end,
	sleep = function(s) return s:get_sleep() end,
	tick = function(s) return s:get_tick() end,
	time = function(s) return s:get_time() end}

local program_setters = {
	quit = function(s, v) s:set_quit(v) end,
	sleep = function(s, v) s:set_sleep(v) end}

Program.getter = function(self, key)
	local programgetterfunc = program_getters[key]
	if programgetterfunc then return programgetterfunc(self) end
	return Class.getter(self, key)
end

Program.setter = function(self, key, value)
	local programsetterfunc = program_setters[key]
	if programsetterfunc then return programsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

--- Pops an event from the event queue.
-- @param clss Program class.
-- @return Event table or nil.
Program.pop_event = function(clss)
	if not __events then return end
	local t = __events[1]
	if t then table.remove(__events, 1) end
	return t
end

--- Pushes an event to the back of the event queue.
-- @param clss Program class.
-- @return event Event table.
Program.push_event = function(clss, event)
	if not __events then __events = {} end
	table.insert(__events, event)
end
