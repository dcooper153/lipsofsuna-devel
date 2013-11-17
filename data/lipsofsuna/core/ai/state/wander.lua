local Vector = require("system/math/vector")

local __vec1 = Vector()

Aistatespec{
	name = "wander",
	calculate = function(self)
		-- Start wandering after waiting for a while.
		if self.state == "wait" and self.state_timer > 10 then
			return 1
		end
		-- Start wandering if nothing else to do.
		return 0.00000002
	end,
	enter = function(self)
		-- Select target point.
		self:choose_wander_target()
	end,
	update = function(self, secs)
		if not self.target then return end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Move towards the target.
		local dist = __vec1:set(self.target):subtract(self.object:get_position()).length
		if dist < 0.5 then
			self.object:set_movement(0)
		else
			self.object:set_movement(1)
		end
	end}
