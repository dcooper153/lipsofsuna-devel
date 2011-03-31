if not Program:load_extension("sound") then
	error("loading extension `sound' failed")
end

local sound_getters = {
	listener_position = function(s) return s:get_listener_position() end,
	listener_rotation = function(s) return s:get_listener_rotation() end,
	listener_velocity = function(s) return s:get_listener_velocity() end}

local sound_setters = {
	listener_position = function(s, v) return s:set_listener_position(v) end,
	listener_rotation = function(s, v) return s:set_listener_rotation(v) end,
	listener_velocity = function(s, v) return s:set_listener_velocity(v) end,
	music = function(s, v) s:set_music(v) end,
	music_fading = function(s, v) s:set_music_fading(v) end,
	music_volume = function(s, v) s:set_music_volume(v) end}

Sound.getter = function(self, key)
	local soundgetterfunc = sound_getters[key]
	if soundgetterfunc then return soundgetterfunc(self) end
	return Class.getter(self, key)
end

Sound.setter = function(self, key, value)
	local soundsetterfunc = sound_setters[key]
	if soundsetterfunc then return soundsetterfunc(self, value) end
	return Class.setter(self, key, value)
end
