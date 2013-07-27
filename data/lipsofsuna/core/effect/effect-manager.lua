--- Manages audiovisual effects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.effect_manager
-- @alias EffectManager

local Class = require("system/class")
local Client = require("core/client/client")
local DamageLabelEffect = require(Mod.path .. "damage-label-effect")
local ParticleEffect = require(Mod.path .. "particle-effect")
local Sound = require("system/sound")
local SoundEffect = require(Mod.path .. "sound-effect")
local SpeechBubbleEffect = require(Mod.path .. "speech-bubble-effect")

--- Manages audiovisual effects.
-- @type EffectManager
local EffectManager = Class("EffectManager")

--- Creates the effect manager.
-- @param clss EffectManager class.
-- @return EffectManager.
EffectManager.new = function(clss)
	local self = Class.new(clss)
	self.speech_bubble_dict = {}
	self.speech_bubble_dict_id = {}
	self.scene_nodes_by_ref = {}
	return self
end

--- Applies a world space quake.
-- @param self EffectManager.
-- @param point Quake point in world space.
-- @param magnitude Quake magnitude.
EffectManager.apply_quake = function(self, point, magnitude)
	if point and magnitude and Client.player_object then
		local dist = (Client.player_object:get_position() - point).length
		local atten = 1 + 0.1 * dist^2
		local quake = math.min(math.max(magnitude / atten, 0), 1)
		if Client.camera_manager then
			Client.camera_manager:quake(quake)
		end
	end
end

--- Creates a damage text effect.
-- @param self EffectManager.
-- @param object Damaged object, or nil.
-- @param point Position in world space, or nil.
-- @param damage Damage amount.
EffectManager.create_damage_text = function(self, object, point, damage)
	if damage > -1 and damage <= 2 then return end
	local effect = DamageLabelEffect(object, point, damage)
	self.speech_bubble_dict[effect] = effect
end

EffectManager.create_speech_bubble = function(self, args)
	-- Try to reuse an existing speech bubble.
	if args.object then
		local bubble = self.speech_bubble_dict_id[args.object:get_id()]
		if bubble then return bubble:add_line(args) end
	end
	-- Create a new speech bubble.
	local bubble = SpeechBubbleEffect(args)
	self.speech_bubble_dict[bubble] = true
	if args.object then
		self.speech_bubble_dict_id[args.object:get_id()] = bubble
	end
end

--- Detaches all the scene nodes.
-- @param self EffectManager.
EffectManager.detach_scene_nodes = function(self)
	for k in pairs(self.scene_nodes_by_ref) do
		k:detach()
	end
	self.scene_nodes_by_ref = {}
end

EffectManager.play = function(self, name)
	if not Client.player_object then return end
	self:play_object(name, Client.player_object)
end

--- Plays a global sound effect, such as an UI sound.
-- @param self EffectManager.
-- @param name Effect spec name.
EffectManager.play_global = function(self, name)
	-- Find the effect.
	local effect = Effectspec:find{name = name}
	if not effect then return end
	-- Create the effect object.
	if effect.sound then
		SoundEffect{
			sound = effect.sound,
			sound_delay = effect.sound_delay,
			sound_pitch = effect.sound_pitch,
			sound_positional = false,
			sound_volume = effect.sound_volume}
	end
	-- Quake the camera.
	if effect.quake and Client.player_object then
		self:apply_quake(Client.player_object:get_position(), effect.quake)
	end
end

EffectManager.play_object = function(self, name, object, node)
	-- Find the effect.
	local effect = Effectspec:find{name = name}
	if not effect then return end
	-- Find the node.
	local p
	local n = node or effect.node
	if n then p = object:find_node{name = n} end
	-- Create the effect object.
	if effect.particle then
		ParticleEffect{
			life = effect.particle_life,
			object = object,
			particle = effect.particle,
			node = p and n,
			rotation_mode = (effect.rotation and "parent" or nil)}
	end
	if effect.sound then
		SoundEffect{
			object = object,
			node = p and n,
			rotation_mode = (effect.rotation and "parent" or nil),
			sound = effect.sound,
			sound_delay = effect.sound_delay,
			sound_pitch = effect.sound_pitch,
			sound_positional = effect.sound_positional,
			sound_volume = effect.sound_volume}
	end
	-- Quake the camera.
	self:apply_quake(object:get_position(), effect.quake)
end

EffectManager.play_world = function(self, name, position)
	-- Find the effect.
	local effect = Effectspec:find{name = name}
	if not effect then return end
	-- Create the effect object.
	if effect.particle then
		ParticleEffect{
			life = effect.particle_life,
			particle = effect.particle,
			position = position}
	end
	if effect.sound then
		SoundEffect{
			position = position,
			sound = effect.sound,
			sound_delay = effect.sound_delay,
			sound_pitch = effect.sound_pitch,
			sound_positional = effect.sound_positional,
			sound_volume = effect.sound_volume}
	end
	-- Quake the camera.
	self:apply_quake(position, effect.quake)
end

EffectManager.speech = function(self, args)
	-- Play the sound effect.
	self:play_object("chat1", args.object)
	-- Create a text bubble.
	local bounds = args.object:get_bounding_box()
	local offset = bounds.point.y + bounds.size.y + 0.5
	self:create_speech_bubble{
		life = 5,
		fade = 1,
		object = args.object,
		offset = Vector(0,offset,0),
		text = args.text,
		text_color = {1,1,1,1},
		text_font = "medium"}
end

EffectManager.update = function(self, secs)
	-- Update scene nodes.
	for k in pairs(self.scene_nodes_by_ref) do
		k:update(secs)
	end
	-- Update text bubbles.
	for k in pairs(self.speech_bubble_dict) do
		k:update(secs)
	end
end

return EffectManager
