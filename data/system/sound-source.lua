--- Sound source.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.sound_source
-- @alias SoundSource

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("sound") then
	error("loading extension `sound' failed")
end

------------------------------------------------------------------------------

--- Sound source.
-- @type SoundSource
local SoundSource = Class("SoundSource")

--- Creates a new sound source.
-- @param clss Sound source class.
-- @param sample Sample name.
-- @param positional True to make the effect positional.
-- @param repeating True to make the effect repeating.
-- @return Sound source.
SoundSource.new = function(clss, sample, positional, repeating)
	local self = Class.new(clss)
	self.__sample = sample
	self.handle = Los.sound_source_new(sample, positional, repeating)
	return self
end

--- Returns true if the source has finished playing its samples.
-- @param self Sound source.
-- @return Boolean.
SoundSource.get_ended = function(self)
	return Los.sound_source_get_ended(self.handle)
end

--- Returns true if the source is looping.
-- @param self Sound source.
-- @return Boolean.
SoundSource.get_looping = function(self)
	return self.__looping
end

--- Sets the looping mode of the source.
-- @param self Sound source.
-- @param value True to loop, false to not.
SoundSource.set_looping = function(self, value)
	self.__looping = value
	Los.sound_source_set_looping(self.handle, value)
end

--- Returns the maximum hearing distance of the source.
-- @param self Sound source.
-- @return Number.
SoundSource.get_max_dist = function(self)
	return self.__maxdist or 100
end

--- Sets the maximum hearing distance of the source.
-- @param self Sound source.
-- @param value Number.
SoundSource.set_max_dist = function(self, value)
	self.__maxdist = value
	Los.sound_source_set_max_dist(self.handle, value)
end

--- Returns the pitch multiplier of the source.
-- @param self Sound source.
-- @return Number.
SoundSource.get_pitch = function(self)
	return self.__pitch or 1
end

--- Sets the pitch multiplier of the source.
-- @param self Sound source.
-- @param value Number, 1 meaning default pitch.
SoundSource.set_pitch = function(self, value)
	self.__pitch = value
	Los.sound_source_set_pitch(self.handle, value)
end

--- Returns true if the source is currently playing.
-- @param self Sound source.
-- @return Boolean.
SoundSource.get_playing = function(self)
	return Los.sound_source_get_playing(self.handle)
end

--- Sets the playback state of the source.
-- @param self Sound source.
-- @param value True to play, false to stop.
SoundSource.set_playing = function(self, value)
	Los.sound_source_set_playing(self.handle, value)
end

--- Returns the world position of the source.
-- @param self Sound source.
-- @return Vector.
SoundSource.get_position = function(self)
	if not self.__position then
		self.__position = Vector()
	end
	return self.__position
end

--- Sets the world position of the source.
-- @param self Sound source.
-- @param value Vector.
SoundSource.set_position = function(self, value)
	self.__position = value
	Los.sound_source_set_position(self.handle, value.handle)
end

--- Returns the reference hearing distance of the source.
-- @param self Sound source.
-- @return Number.
SoundSource.get_ref_dist = function(self)
	return self.__refdist or 1
end

--- Sets the reference hearing distance of the source.
-- @param self Sound source.
-- @param value Number.
SoundSource.set_ref_dist = function(self, value)
	self.__refdist = value
	Los.sound_source_set_ref_dist(self.handle, value)
end

--- Returns the roll-off rate of the source.
-- @param self Sound source.
-- @return Number.
SoundSource.get_rolloff = function(self)
	return self.__rolloff or 1
end

--- Sets the roll-off rate of the source.
-- @param self Sound source.
-- @param value Number.
SoundSource.set_rolloff = function(self, value)
	self.__rolloff = value
	Los.sound_source_set_rolloff(self.handle, value)
end

--- Returns the name of the sample used by the source.
-- @param self Sound source.
-- @return String.
SoundSource.get_sample = function(self)
	return self.__sample
end

--- Returns the linear velocity of the source.
-- @param self Sound source.
-- @return Vector.
SoundSource.get_velocity = function(self)
	if not self.__velocity then
		self.__velocity = Vector()
	end
	return self.__velocity
end

--- Sets the linear velocity of the source.
-- @param self Sound source.
-- @param value Vector.
SoundSource.set_velocity = function(self, value)
	self.__velocity = value
	Los.sound_source_set_velocity(self.handle, value.handle)
end

--- Returns the volume multiplier of the source.
-- @param self Sound source.
-- @return Number.
SoundSource.get_volume = function(self)
	return self.__volume or 1
end

--- Sets the volume multiplier of the source.
-- @param self Sound source.
-- @param value Number, 1 meaning default volume.
SoundSource.set_volume = function(self, value)
	self.__volume = value
	Los.sound_source_set_volume(self.handle, value)
end

return SoundSource
