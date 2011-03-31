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
