--- Audiovisual effect specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.effect
-- @alias EffectSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Audiovisual effect specification.
-- @type EffectSpec
local EffectSpec = Spec:register("EffectSpec", "effect", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "light", type = "boolean", description = "True to enable light source creation."},
	{name = "light_ambient", type = "color", description = "Ambient color of the light source."},
	{name = "light_diffuse", type = "color", description = "Diffuse color of the light source."},
	{name = "light_equation", type = "list", list = {type = "number"}, description = "Attenuation equation of the light source."},
	{name = "model", type = "string", description = "Model to render."},
	{name = "node", type = "string", description = "Name of the anchor when emitted by an object."},
	{name = "particle", type = "string", description = "Particle effect to show."},
	{name = "particle_life", type = "number", description = "Life-time of particle effects, in seconds."},
	{name = "quake", type = "number", description = "Camera quake amount."},
	{name = "rotation", type = "string", description = "False to not inherit rotation from the parent object."},
	{name = "sound", type = "string", description = "Sound effect to play."},
	{name = "sound_delay", type = "string", description = "Delay of the sound start, in seconds."},
	{name = "sound_dist_max", type = "number", description = "Maximum hearing distance of the sound effect."},
	{name = "sound_dist_ref", type = "number", description = "Reference hearing distance of the sound effect."},
	{name = "sound_loop", type = "boolean", description = "True to make the sound effect loop."},
	{name = "sound_pitch_min", type = "number", default = 1, description = "Minimum pitch of the sound effect."},
	{name = "sound_pitch_max", type = "number", default = 1, description = "Maximum pitch of the sound effect."},
	{name = "sound_rolloff", type = "number", description = "Roll-off rate of the sound effect."},
	{name = "sound_volume", type = "number", description = "Volume level of the sound effect."},
	{name = "sound_positional", type = "boolean", description = "False to make the sound non-positional."}
})

--- Registers a new effect spec.
-- @param clss EffectSpec class.
-- @param args Arguments.
-- @return New effect spec.
EffectSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the pitch of the sound.
-- @param self EffectSpec.
-- @return Pitch multiplier.
EffectSpec.get_sound_pitch = function(self)
	local min = self.sound_pitch_min
	local max = self.sound_pitch_max
	if min >= max then return min end
	return min + (max - min) * math.random()
end

return EffectSpec
