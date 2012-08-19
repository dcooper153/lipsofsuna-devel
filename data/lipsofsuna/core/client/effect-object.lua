local Class = require("system/class")

local EffectObject = Class()

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

return EffectObject
