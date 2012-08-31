--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.model_effect
-- @alias ModelEffect

local Class = require("system/class")
local EffectObject = require("core/client/effect-object")
local RenderObject = require("system/render-object")
local SpeedlineEffect = require("core/client/speedline-effect")

--- TODO:doc
-- @type ModelEffect
local ModelEffect = Class("ModelEffect", EffectObject)

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
	self.speedline = SpeedlineEffect(args)
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

return ModelEffect


