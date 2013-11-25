--- Anchorable censorship effect.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.model_effect
-- @alias CensorshipEffect

local Billboard = require("system/billboard")
local Class = require("system/class")
local EffectObject = require("core/effect/effect-object")
local RenderObject = require("system/render-object")
local Vector = require("system/math/vector")

--- Anchorable censorship effect.
-- @type CensorshipEffect
local CensorshipEffect = Class("CensorshipEffect", EffectObject)

--- Creates a new model effect.
-- @param clss CensorshipEffect class.
-- @param parent: Parent object.
-- @param parent_node: Parent node name.
-- @return CensorshipEffect.
CensorshipEffect.new = function(clss, parent, parent_node)
	local self = EffectObject.new(clss, {parent = parent, parent_node = parent_node})
	self.billboard = Billboard()
	self.billboard:set_render_queue("main")
	self.billboard:set_render_queue("8")
	self.billboard:set_size(0.2, 0.2)
	self.billboard:set_material("censorship1")
	self.billboard:add(Vector())
	self.billboard:set_visible(true)
	return self
end

--- Removes the effect from the scene.
-- @param self EffectObject.
CensorshipEffect.detach = function(self)
	-- Detach the render object.
	self.billboard:set_visible(false)
	-- Call the base class.
	EffectObject.detach(self)
end

--- Removes the effect from the scene after a while.
-- @param self EffectObject.
CensorshipEffect.detach_delayed = function(self)
	self.__detach_timer = 1
end

--- Updates the effect.
-- @param self EffectObject.
-- @param secs Seconds since the last update.
CensorshipEffect.update = function(self, secs)
	if self.__detach_timer then
		self.__detach_timer = self.__detach_timer - secs
		if self.__detach_timer <= 0 then
			return self:detach()
		end
	end
	self:__recalculate_transform()
	EffectObject.update(self, secs)
end

--- Sets the position of the effect.
-- @param self EffectObject.
-- @param v Vector.
CensorshipEffect.set_position = function(self, v)
	EffectObject.set_position(self, v)
	self:__recalculate_transform()
end

--- Sets the rotation of the effect.
-- @param self EffectObject.
-- @param v Quaternion.
CensorshipEffect.set_rotation = function(self, v)
	EffectObject.set_rotation(self, v)
end

CensorshipEffect.__recalculate_transform = function(self)
	local r1,r2 = Client.camera_manager.camera:get_picking_ray()
	local d = r2:subtract(r1):normalize():multiply(0.1)
	self.billboard:set_position(self:get_position() - d)
	self.billboard:set_rotation(self:get_rotation())
end

return CensorshipEffect
