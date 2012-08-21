local Class = require("system/class")
require(Mod.path .. "spec")

Effectspec = Class("Effectspec", Spec)
Effectspec.type = "effect"
Effectspec.dict_id = {}
Effectspec.dict_cat = {}
Effectspec.dict_name = {}
Effectspec.introspect = Introspect{
	name = "Effectspec",
	fields = {
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
		{name = "sound_pitch_min", type = "number", description = "Minimum pitch of the sound effect."},
		{name = "sound_pitch_max", type = "number", description = "Maximum pitch of the sound effect."},
		{name = "sound_volume", type = "number", description = "Volume level of the sound effect."},
		{name = "sound_positional", type = "boolean", description = "False to make the sound non-positional."}
	}}

--- Registers a new effect spec.
-- @param clss Effectspec class.
-- @param args Arguments.
-- @return New effect spec.
Effectspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end
