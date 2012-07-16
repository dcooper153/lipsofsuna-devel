EffectObject = Class()

--- Creates a new effect.
-- @param clss EffectObject class.
-- @param args Arguments.<ul>
--   <li>life: Life time in seconds.</li>
--   <li>parent: Parent object, or nil.</li>
--   <li>parent_node: Parent node name, or nil.</li>
--   <li>position: Position in world space, or nil if parent is used.</li>
--   <li>position_mode: One of "node-node", "node" or nil.</li>
--   <li>rotation_mode: One of "node-node", "node" or nil.</li></ul>
-- @return Effect object.
EffectObject.new = function(clss, args)
	local self = Class.new(clss)
	self.life = args.life
	self.parent = args.parent or args.object
	self.parent_node = args.parent_node or args.node
	self.position_mode = args.position_mode or "node"
	self.rotation_mode = args.rotation_mode or "node"
	Game.scene_nodes_by_ref[self] = true
	self.__visible = true
	self.__initial_position = args.position
	return self
end

EffectObject.detach = function(self)
	Game.scene_nodes_by_ref[self] = nil
end

EffectObject.unparent = function(self)
	self.parent = nil
	self:detach()
end

EffectObject.update = function(self, secs)
	if self.__initial_position then
		self:set_position(self.__initial_position)
		self.__initial_position = nil
	end
	if self.life then
		self.life = self.life - secs
		if self.life < 0 then return self:detach() end
	end
	if self.parent and not self.parent:get_visible() then
		self:unparent()
	end
	self:update_transform(secs)
end

EffectObject.update_transform = function(self, secs)
	if not self.parent then return end
	-- Update the position.
	if self.position_mode == "node-node" then
		local p,r = self.parent:find_node{name = self.parent_node, space = "world"}
		if not p then p = self.parent:get_position() end
		if self.model_anchor and self.render then
			local h,s = self.render:find_node{name = self.model_anchor}
			if h then p:subtract(r * (s.conjugate * h)) end
		end
		self:set_position(p)
	elseif self.position_mode == "node" then
		local p = self.parent:find_node{name = self.parent_node}
		if p then
			self:set_position(self.parent:get_position() + self.parent:get_rotation() * p)
		else
			self:set_position(self.parent:get_position())
		end
	else
		self:set_position(self.parent:get_position())
	end
	-- Update the rotation.
	if self.rotation_mode == "node-node" then
		local p,r = self.parent:find_node{name = self.parent_node, space = "world"}
		if not r then r = self.parent:get_rotation() end
		if self.model_anchor and self.render then
			local h,s = self.render:find_node{name = self.model_anchor}
			if s then r = r * s.conjugate end
		else
			r = r * Quaternion{axis = Vector(0,1,0), angle = math.pi/2}
		end
		self:set_rotation(r)
	elseif self.rotation_mode == "node" then
		local p,r = self.parent:find_node{name = self.parent_node}
		if r then
			self:set_rotation(self.parent:get_rotation() * r)
		else
			self:set_rotation(self.parent:get_rotation())
		end
	else
		self:set_rotation(self.parent:get_rotation())
	end
end

EffectObject.get_position = function(self)
	return self.__position or Vector()
end

EffectObject.set_position = function(self, v)
	self.__position = v
end

EffectObject.get_visible = function(self)
	return self.__visible
end

EffectObject.set_visible = function(self, v)
	self.__visible = v
end

EffectObject.get_rotation = function(self)
	return self.__rotation or Quaternion()
end

EffectObject.set_rotation = function(self, v)
	self.__rotation = v
end

------------------------------------------------------------------------------

ModelEffect = Class(EffectObject)

--- Creates a new model effect.
-- @param clss Model effect class.
-- @param args Arguments.<ul>
--   <li>Arguments inherited from EffectObject.</li>
--   <li>model: Render model.</li>
--   <li>model_node: Model node name, or nil.</li></ul>
-- @return Model effect.
ModelEffect.new = function(clss, args)
	local self = EffectObject.new(clss, args)
	self.model = args.model:get_render()
	self.model_node = args.model_node
	self.render = RenderObject()
	self.render:set_visible(true)
	self.render:add_model(self.model)
	return self
end

ModelEffect.add_special_effects = function(self, list)
	if not self.specia_effects then
		self.special_effects = {}
	end
	for k,v in pairs(list) do
		table.insert(self.special_effects, v)
	end
end

ModelEffect.add_speedline = function(self, args)
	-- Stop the old speedline
	if self.speedline then
		self.speedline.object = nil
		self.speedline = nil
	end
	-- Create the new speedline
	if not args then args = {} end
	args.object = self
	self.speedline = Speedline(args)
end

ModelEffect.detach = function(self)
	-- Detach the render object.
	self.render:set_visible(false)
	-- Detach special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do v:unparent() end
		self.special_effects = nil
	end
	-- Call the base class.
	EffectObject.detach(self)
end

ModelEffect.find_node = function(self, ...)
	return self.render:find_node(...)
end

ModelEffect.set_position = function(self, v)
	self.render:set_position(v)
	EffectObject.set_position(self, v)
end

ModelEffect.set_rotation = function(self, v)
	self.render:set_rotation(v)
	EffectObject.set_rotation(self, v)
end

------------------------------------------------------------------------------

ParticleEffect = Class(EffectObject)

ParticleEffect.new = function(clss, args)
	local self = EffectObject.new(clss, args)
	self.render = RenderObject()
	self.render:particle_animation{loop = false}
	self.render:set_particle(args.particle)
	self.render:set_visible(true)
	return self
end

ParticleEffect.detach = function(self)
	self.render:set_visible(false)
	EffectObject.detach(self)
end

ParticleEffect.unparent = function(self)
	self.render:set_particle_emitting(false)
	self.unparent_timer = 5
	self.parent = nil
end

ParticleEffect.update = function(self, secs)
	if self.unparent_timer then
		self.unparent_timer = self.unparent_timer - secs
		if self.unparent_timer <= 0 then
			return self:detach()
		end
	end
	EffectObject.update(self, secs)
end

ParticleEffect.set_position = function(self, v)
	self.render:set_position(v)
	EffectObject.set_position(self, v)
end

ParticleEffect.set_rotation = function(self, v)
	self.render:set_rotation(v)
	EffectObject.set_rotation(self, v)
end

------------------------------------------------------------------------------

LightEffect = Class(EffectObject)

LightEffect.new = function(clss, args)
	local self = EffectObject.new(clss, args)
	self.light = Light()
	self.light.ambient = args.ambient
	self.light.diffuse = args.diffuse
	self.light.equation = args.equation
	self.light.enabled = true
	return self
end

LightEffect.detach = function(self)
	self.light.enabled = false
	EffectObject.detach(self)
end

LightEffect.set_position = function(self, v)
	self.light.position = v
	EffectObject.set_position(self, v)
end

LightEffect.set_rotation = function(self, v)
	self.light.rotation = v
	EffectObject.set_rotation(self, v)
end

------------------------------------------------------------------------------

SoundEffect = Class(EffectObject)

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
