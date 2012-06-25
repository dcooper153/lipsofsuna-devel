BaseObject = Class(Object)
BaseObject.class_name = "BaseObject"

--- Creates a new object.
-- @param clss Base object class.
-- @param args Arguments.
-- @returns New object.
BaseObject.new = function(clss, args)
	local self = Object.new(clss, args)
	return self
end

--- Detaches the object and its anchors from the scene.
-- @param self Object.
BaseObject.detach = function(self)
	-- Hide special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do
			if v.enabled then
				v.enabled = false
			else
				v.realized = false
			end
		end
		self.special_effects = nil
	end
	-- Stop the speed line.
	if self.speedline then
		self.speedline.object = nil
	end
	-- Hide self.
	self.realized = false
	Object.dict_active[self] = nil
end

--- Updates the object.
-- @param self Object.
-- @param secs Seconds since the last update.
BaseObject.update = function(self, secs)
	if not self.realized then return end
	if self.special_effects then
		for k,v in pairs(self.special_effects) do
			local p = self.position
			local r = self.rotation
			if v.node then
				local np,nr = self:find_node{name = v.node}
				if np then
					p = p + r * np
					r = r * nr
				end
			end
			if v.offset then p = p + v.offset end
			if v.rotate then v.rotation = r end
			v.position = p
		end
	end
end

-- FIXME: Shouldn't be here.
Object.physics_position_correction = Vector(0, 0, 0)
if not Object.animate then
	Object.animate = function() end
end
if not Object.find_node then
	Object.find_node = function() end
end
if not Object.get_animation then
	Object.get_animation = function() end
end
