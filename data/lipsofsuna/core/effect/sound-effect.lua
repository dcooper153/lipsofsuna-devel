--- Sound effect anchor.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.sound_effect
-- @alias SoundEffect

local Class = require("system/class")
local EffectObject = require(Mod.path .. "effect-object")
local SoundSource = require("system/sound-source")

--- Sound effect anchor.
-- @type SoundEffect
local SoundEffect = Class("SoundEffect", EffectObject)

--- Creates a new effect.
-- @param clss EffectObject class.
-- @param args Arguments.<ul>
--   <li>Arguments inherited from EffectObject.</li>
--   <li>sound: Sound effect name.</li>
--   <li>sound_delay: Sound delay in seconds.</li>
--   <li>sound_pitch: Sound effect pitch range.</li>
--   <li>sound_positional: False to make the sound non-positional.</li></ul>
-- @return Effect object.
SoundEffect.new = function(clss, args)
	local self = EffectObject.new(clss, args)
	if not args.sound_delay then
		self:create_source(args)
	else
		self.delay = args.sound_delay
		self.args = args
	end
	return self
end

--- Creates the actual sound source and starts playback.
-- @param self SoundEffect.
-- @param args Arguments as given to the constructor.
SoundEffect.create_source = function(self, args)
	self.source = SoundSource(args.sound, args.sound_positional)
	self.source:set_volume((args.sound_volume or 1) * Client.options.sound_volume)
	if args.sound_pitch then
		self.source:set_pitch(args.sound_pitch)
	end
	if args.sound_loop then
		self.source:set_looping(true)
	end
	if args.sound_dist_max then
		self.source:set_ref_dist(args.sound_dist_max)
	end
	if args.sound_dist_ref then
		self.source:set_ref_dist(args.sound_dist_ref)
	end
	if args.sound_rolloff then
		self.source:set_rolloff(args.sound_rolloff)
	end
	if self.__queued_position then
		self.source:set_position(self.__queued_position)
		self.__queued_position = nil
	end
	self.source:set_playing(true)
end

--- Detaches the sound effect from the world.
-- @param self SoundEffect.
SoundEffect.detach = function(self)
	EffectObject.detach(self)
	if self.source then
		self.source:set_playing(false)
	end
	self.delay = nil
end

--- Updates the sound source.
-- @param self SoundEffect.
-- @param secs Seconds since the last update.
SoundEffect.update = function(self, secs)
	-- Initialize after the delay.
	if self.delay then
		self.delay = self.delay - secs
		if self.delay <= 0 then
			self:create_source(self.args)
			self.delay = nil
		end
	end
	-- Detach after finished playing.
	if self.source and self.source:get_ended() then
		self:detach()
		return
	end
	-- Call the base class.
	EffectObject.update(self, secs)
end

--- Sets the world space position of the sound effect.<br/>
--
-- Sets the position of the effect if playing has started. If the playback
-- is currently being delayed, the position is queued and set when the
-- playback starts.
--
-- @param self SoundEffect.
-- @param v Vector.
SoundEffect.set_position = function(self, v)
	if self.source then
		self.source:set_position(v)
	else
		self.__queued_position = v
	end
	EffectObject.set_position(self, v)
end

return SoundEffect
