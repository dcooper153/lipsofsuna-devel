require "client/objects/effectobject"

Effect = Class()

Effect.play = function(clss, name)
	if not Client.player_object then return end
	clss:play_object(name, Client.player_object)
end

--- Plays a global sound effect, such as an UI sound.
-- @param clss Effect class.
-- @param name Effect spec name.
Effect.play_global = function(clss, name)
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
		Client:apply_quake(Client.player_object.position, effect.quake)
	end
end

Effect.play_object = function(clss, name, object, node)
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
	Client:apply_quake(object.position, effect.quake)
end

Effect.play_world = function(clss, name, position)
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
	Client:apply_quake(Vector(x,y,z), effect.quake)
end
