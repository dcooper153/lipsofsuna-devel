local Class = require("system/class")
local EffectObject = require(Mod.path .. "effect-object")

local ParticleEffect = Class(EffectObject)

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

return ParticleEffect
