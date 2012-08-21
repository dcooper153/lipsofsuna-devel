local Class = require("system/class")
local EffectObject = require(Mod.path .. "effect-object")
local SoundSource = require("system/sound-source")

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

SoundEffect.create_source = function(self, args)
	self.source = SoundSource(args.sound, args.sound_positional)
	self.source:set_volume((args.sound_volume or 1) * Client.options.sound_volume)
	if args.sound_pitch then
		self.source:set_pitch(1 + args.sound_pitch * (math.random() - 0.5))
	end
	self.source:set_playing(true)
end

SoundEffect.detach = function(self)
	EffectObject.detach(self)
	if self.source then
		self.source:set_playing(false)
	end
	self.delay = nil
end

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

SoundEffect.set_position = function(self, v)
	if self.source then
		self.source:set_position(v)
	end
	EffectObject.set_position(self, v)
end

return SoundEffect
