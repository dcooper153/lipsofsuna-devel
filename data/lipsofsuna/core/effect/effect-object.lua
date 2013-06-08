--- Base class for anchorable effects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.effect_object
-- @alias EffectObject

local Class = require("system/class")

--- Base class for anchorable effects.
-- @type EffectObject
local EffectObject = Class("EffectObject")

--- Creates a new effect.
-- @param clss EffectObject class.
-- @param args Arguments.<ul>
--   <li>life: Life time in seconds.</li>
--   <li>parent: Parent object, or nil.</li>
--   <li>parent_node: Parent node name, or nil.</li>
--   <li>position: Position in world space, or nil if parent is used.</li>
--   <li>position_mode: One of "node-node", "node" or nil.</li>
--   <li>rotation_mode: One of "node-node", "node" or nil.</li></ul>
-- @return EffectObject.
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

--- Removes the effect from the scene.
-- @param self EffectObject.
EffectObject.detach = function(self)
	Game.scene_nodes_by_ref[self] = nil
end

--- Transforms a position vector from the local coordinate space to global.
-- @param self EffectObject.
-- @param point Vector.
-- @return Vector.
EffectObject.transform_local_to_global = function(self, point)
	local pos = self:get_position()
	if not point then return pos end
	local rot = self:get_rotation()
	return pos:copy():add(rot * point)
end

--- Detaches the effect from its parent.
-- @param self EffectObject.
EffectObject.unparent = function(self)
	self.parent = nil
	self:detach()
end

--- Updates the effect.
-- @param self EffectObject.
-- @param secs Seconds since the last update.
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

--- Updates the transformation of the effect.
-- @param self EffectObject.
-- @param secs Seconds since the last update.
EffectObject.update_transform = function(self, secs)
	if not self.parent then return end
	-- Update the position.
	if self.position_mode == "node-node" then
		local p,r = self.parent:find_node{name = self.parent_node, space = "world"}
		if not p then p = self.parent:get_position() end
		if self.model_anchor and self.render then
			local h,s = self.render:find_node{name = self.model_anchor}
			if h then
				p:subtract(h:transform(s.conjugate):transform(r))
			end
		end
		self:set_position(p)
	elseif self.position_mode == "node" then
		local p = self.parent:find_node{name = self.parent_node}
		if p then
			p:transform(self.parent:get_rotation(), self.parent:get_position())
			self:set_position(p)
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
			r:concat(Quaternion{axis = Vector(0,1,0), angle = math.pi/2})
		end
		self:set_rotation(r)
	elseif self.rotation_mode == "node" then
		local p,r = self.parent:find_node{name = self.parent_node}
		if r then
			self:set_rotation(self.parent:get_rotation():copy():concat(r))
		else
			self:set_rotation(self.parent:get_rotation())
		end
	else
		self:set_rotation(self.parent:get_rotation())
	end
end

--- Gets the position of the effect.
-- @param self EffectObject.
-- @return Vector.
EffectObject.get_position = function(self)
	return self.__position or Vector()
end

--- Sets the position of the effect.
-- @param self EffectObject.
-- @param v Vector.
EffectObject.set_position = function(self, v)
	self.__position = v
end

--- Gets the visibility of the effect.
-- @param self EffectObject.
-- @return Boolean.
EffectObject.get_visible = function(self)
	return self.__visible
end

--- Sets the visibility of the effect.
-- @param self EffectObject.
-- @param v Boolean.
EffectObject.set_visible = function(self, v)
	self.__visible = v
end

--- Gets the rotation of the effect.
-- @param self EffectObject.
-- @return Quaternion.
EffectObject.get_rotation = function(self)
	return self.__rotation or Quaternion()
end

--- Sets the rotation of the effect.
-- @param self EffectObject.
-- @param v Quaternion.
EffectObject.set_rotation = function(self, v)
	self.__rotation = v
end

return EffectObject
