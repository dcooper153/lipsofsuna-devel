Effect.play = function(clss, name)
	EffectObject{
		object = Player.object,
		sound = name,
		sound_pitch = 1,
		realized = true}
end

EffectObject = Class(Object)

--- Creates a new effect.
-- @param clss EffectObject class.
-- @param args Arguments.<ul>
--   <li>font: Font name or nil.</li>
--   <li>font_color: Font color table or nil.</li>
--   <li>life: Life time in seconds or nil.</li>
--   <li>model: Particle effect name or nil.</li>
--   <li>object: Parent object or nil.</li>
--   <li>sound: Sound effect name or nil.</li>
--   <li>sound_pitch: Sound effect pitch factor or nil.</li>
--   <li>text: Text effect string or nil.</li>
--   <li>velocity: Velocity vector or nil.</li></ul>
-- @return Object.
EffectObject.new = function(clss, args)
	local life = args.life or 10
	local parent = args.object
	local velocity = args.velocity or Vector()
	-- Attach a model effect.
	local self = Object.new(clss, args)
	self:particle_animation{loop = false}
	-- Attach a sound effect.
	if args.sound then
		local pitch = args.sound_pitch or (0.8+0.4*math.random())
		Sound:effect{object = self, effect = args.sound, pitch = pitch}
	end
	-- Attach a text effect.
	if args.text then
		Speech:add{object = self, diffuse = args.text_color, font = args.text_font,
			message = args.text, fade_time = args.text_fade_time, life_time = args.life}
	end
	-- Copy parent transformation.
	if parent then
		self.position = parent.position + self.position
		self.rotation = parent.rotation * self.rotation
	end
	-- Update in a thread until the effect ends.
	Thread(function()
		local t = 0
		local moved = parent and (self.position - parent.position) or self.position
		while t < life and (not parent or parent.realized) do
			local secs = coroutine.yield()
			moved = moved + velocity * secs
			if parent then
				self.position = parent.position + moved
				self.rotation = parent.rotation
			elseif args.velocity then
				self.position = moved
			end
			t = t + secs
		end
		self.realized = false
	end)
	return self
end
