local Class = require("system/class")
local EffectObject = require(Mod.path .. "effect-object")

local LightEffect = Class(EffectObject)

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

return LightEffect
