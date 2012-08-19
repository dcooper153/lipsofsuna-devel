local Class = require("system/class")
local ParticleEffect = require(Mod.path .. "particle-effect")
local SoundEffect = require(Mod.path .. "sound-effect")
local SpeechBubbleEffect = require(Mod.path .. "speech-bubble-effect")

local EffectManager = Class()

EffectManager.new = function(clss)
	local self = Class.new(clss)
	self.music_mode = "none"
	self.speech_bubble_dict = {}
	self.speech_bubble_dict_id = {}
	return self
end

--- Applies a world space quake.
-- @param self EffectManager.
-- @param point Quake point in world space.
-- @param magnitude Quake magnitude.
EffectManager.apply_quake = function(self, point, magnitude)
	if point and magnitude and Client.player_object then
		local dist = (Client.player_object.position - point).length
		local quake = math.min(math.max(magnitude / (0.05 * dist * dist + 0.5), 0), 1)
		Client.camera1.quake = math.max(Client.camera1.quake or 0, quake)
		Client.camera3.quake = math.max(Client.camera3.quake or 0, quake)
	end
end

EffectManager.create_speech_bubble = function(self, args)
	-- Try to reuse an existing speech bubble.
	if args.object then
		local bubble = self.speech_bubble_dict_id[args.object.id]
		if bubble then return bubble:add_line(args) end
	end
	-- Create a new speech bubble.
	local bubble = SpeechBubbleEffect(args)
	self.speech_bubble_dict[bubble] = true
	if args.object then
		self.speech_bubble_dict_id[args.object.id] = bubble
	end
end

--- Switches the music track.
-- @param self EffectManager.
EffectManager.cycle_music_track = function(self)
	local modes = {
		boss = {"fairytale7", "fairytale9"},
		char = {"fairytale2", "fairytale10", "fairytale8"},
		game = {"fairytale1", "fairytale3", "fairytale4", "fairytale5",
		        "fairytale6", "fairytale11", "fairytale12", "fairytale13"},
		intro = {"fairytale7"},
		menu = {"menu1"}};
	if not self.music_mode then return end
	local tracks = modes[self.music_mode];
	if not tracks then return end
	Sound.music_fading = 2.0
	Sound.music_looping = (#tracks > 1)
	Sound.music_volume = Client.options.music_volume
	Sound.music = tracks[math.random(1, #tracks)]
end

--- Switches the music track.
-- @param self EffectManager.
-- @param mode Music mode. ("boss"/"char"/"game"/"menu")
EffectManager.switch_music_track = function(self, mode)
	if not mode then return end
	if mode == self.music_mode then return end
	self.music_mode = mode
	self:cycle_music_track()
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
		self:apply_quake(Client.player_object.position, effect.quake)
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
	self:apply_quake(object.position, effect.quake)
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
	self:apply_quake(Vector(x,y,z), effect.quake)
end

EffectManager.update = function(self, secs)
	-- Update text bubbles.
	for k in pairs(self.speech_bubble_dict) do
		k:update(secs)
	end
end

return EffectManager
