if not Program:load_extension("graphics") then
	error("loading extension `graphics' failed")
end

local client_getters = {
	cursor_pos = function(s) return s:get_cursor_pos() end,
	fps = function(s) return s:get_fps() end,
	moving = function(s) return s:get_moving() end,
	video_mode = function(s) return s:get_video_mode() end,
	video_modes = function(s) return s:get_video_modes() end}

local client_setters = {
	moving = function(s, v) s:set_moving(v) end,
	title = function(s, v) s:set_title(v) end}

Client.getter = function(self, key)
	local clientgetterfunc = client_getters[key]
	if clientgetterfunc then return clientgetterfunc(self) end
	return Class.getter(self, key)
end

Client.setter = function(self, key, value)
	local clientsetterfunc = client_setters[key]
	if clientsetterfunc then return clientsetterfunc(self, value) end
	return Class.setter(self, key, value)
end
