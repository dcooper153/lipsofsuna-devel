local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local SimulationObject = require("core/objects/simulation")
local Vector = require("system/math/vector")

local EditorObject = Class("EditorObject", SimulationObject)

EditorObject.new = function(clss)
	local self = SimulationObject.new(clss)
	return self
end

EditorObject.move = function(self, value, step)
	self:set_position(self:get_position() + value * step, true)
end

EditorObject.snap = function(self, pstep, rstep)
	-- Position.
	local v = self:get_position() + Vector(pstep,pstep,pstep) * 0.5
	v.x = v.x - v.x % pstep
	v.y = v.y - v.y % pstep
	v.z = v.z - v.z % pstep
	self:set_position(v)
	-- Rotation.
	local r = self:get_rotation().euler[1] + 0.5 * rstep
	self:set_rotation(Quaternion{euler = {r - r % rstep, 0, 0}})
end

--- Rotates the object with step snapping.
-- @param self Object.
-- @param value Step size.
-- @param steps Number of steps per revolution.
EditorObject.rotate = function(self, value, steps)
	local s = 2 * math.pi / steps
	local e = self:get_rotation().euler
	e[1] = math.floor(e[1] / s + value + 0.5) * s
	self:set_rotation(Quaternion{euler = e})
end

EditorObject.update = function(self, secs)
	SimulationObject.update(self, secs)
	self.render:update(secs)
end

EditorObject.set_spec = function(self, v)
	SimulationObject.set_spec(self, v)
	self:set_model_name(v.model)
	self.render:request_model_rebuild()
	if v.type == "actor" then
		self.render:add_animation("idle")
	end
end

EditorObject.set_visible = function(self, v)
	SimulationObject.set_visible(self, v)
	if v then
		self.render:init(self)
	end
end

EditorObject.get_beheaded = function(self)
	return false
end

return EditorObject
