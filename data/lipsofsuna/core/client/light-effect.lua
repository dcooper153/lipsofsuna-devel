local Class = require("system/class")
local EffectObject = require("core/client/effect-object")
local Light = require("system/light")

local LightEffect = Class("LightEffect", EffectObject)

LightEffect.new = function(clss, args)
	local self = EffectObject.new(clss, args)
	self.light = Light()
	self.light:set_ambient(args.ambient)
	self.light:set_diffuse(args.diffuse)
	self.light:set_equation(args.equation)
	self.light:set_enabled(true)
	return self
end

LightEffect.detach = function(self)
	self.light:set_enabled(false)
	EffectObject.detach(self)
end

LightEffect.set_position = function(self, v)
	self.light:set_position(v)
	EffectObject.set_position(self, v)
end

LightEffect.set_rotation = function(self, v)
	self.light:set_rotation(v)
	EffectObject.set_rotation(self, v)
end

return LightEffect
