--- Anchorable softbody.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.model_effect
-- @alias SoftbodyEffect

local Class = require("system/class")
local EffectObject = require("core/effect/effect-object")
local Softbody = require("system/softbody")

--- Anchorable softbody.
-- @type SoftbodyEffect
local SoftbodyEffect = Class("SoftbodyEffect", EffectObject)

--- Creates a new softbody effect.
-- @param clss SoftbodyEffect class.
-- @param parent Parent object.
-- @param parent_node Parent node name.
-- @param model Model.
-- @param params Softbody parameters.
-- @return SoftbodyEffect.
SoftbodyEffect.new = function(clss, parent, parent_node, model, params)
	local self = EffectObject.new(clss, {
		parent = parent,
		parent_node = parent_node,
		position_mode = "node-node",
		rotation_mode = "node-node"})
	self.model = model
	self.softbody = Softbody(self.model, params)
	self.softbody:set_collision_group(1)
	self.softbody:set_collision_mask(0xFF00)
	self.softbody:set_visible(true)
	return self
end

--- Removes the effect from the scene.
-- @param self EffectObject.
SoftbodyEffect.detach = function(self)
	-- Detach the render object.
	self.softbody:set_visible(false)
	-- Call the base class.
	EffectObject.detach(self)
end

--- Updates the effect.
-- @param self EffectObject.
-- @param secs Seconds since the last update.
SoftbodyEffect.update = function(self, secs)
	EffectObject.update(self, secs)
	self.softbody:update(secs)
end

--- Sets the position of the effect.
-- @param self EffectObject.
-- @param v Vector.
SoftbodyEffect.set_position = function(self, v)
	self.softbody:set_position(v)
	EffectObject.set_position(self, v)
end

--- Sets the rotation of the effect.
-- @param self EffectObject.
-- @param v Quaternion.
SoftbodyEffect.set_rotation = function(self, v)
	self.softbody:set_rotation(v)
	EffectObject.set_rotation(self, v)
end

return SoftbodyEffect
