local Class = require("system/class")

if not Los.program_load_extension("sound") then
	error("loading extension `sound' failed")
end

------------------------------------------------------------------------------

local Sound = Class("Sound")

--- Gets the position of the listener.
-- @param self Music class.
-- @return Vector.
Sound.get_listener_position = function(self)
	return Vector:new_from_handle(Los.sound_get_listener_position())
end

--- Sets the position of the listener.
-- @param self Music class.
-- @param v Vector.
Sound.set_listener_position = function(self, v)
	return Los.sound_set_listener_position(v.handle)
end

--- Gets the rotation of the listener.
-- @param self Music class.
-- @return Quaternion.
Sound.get_listener_rotation = function(self)
	return Vector:new_from_handle(Los.sound_get_listener_rotation())
end

--- Sets the rotation of the listener.
-- @param self Music class.
-- @param v Quaternion.
Sound.set_listener_rotation = function(self, v)
	return Los.sound_set_listener_rotation(v.handle)
end

--- Gets the velocity of the listener.
-- @param self Music class.
-- @return Vector.
Sound.get_listener_velocity = function(self)
	return Vector:new_from_handle(Los.sound_get_listener_velocity())
end

--- Sets the velocity of the listener.
-- @param self Music class.
-- @param v Vector.
Sound.set_listener_velocity = function(self, v)
	return Los.sound_set_listener_velocity(v.handle)
end

--- Sets the music track name.
-- @param self Music class.
-- @param v String.
Sound.set_music = function(self, v)
	Los.sound_set_music(v)
end

--- Sets the music fading time, in seconds.
-- @param self Music class.
-- @param v Number.
Sound.set_music_fading = function(self, v)
	Los.sound_set_music_fading(v)
end

--- Gets the music offset, in seconds.
-- @param self Music class.
-- @param v Number.
Sound.get_music_offset = function(self)
	return Los.sound_get_music_offset()
end

--- Sets the music offset, in seconds.
-- @param self Music class.
-- @param v Number.
Sound.set_music_offset = function(self, v)
	Los.sound_set_music_offset(v)
end

--- Sets the music volume.
-- @param self Music class.
-- @param v Number.
Sound.set_music_volume = function(self, v)
	Los.sound_set_music_volume(v)
end

return Sound
