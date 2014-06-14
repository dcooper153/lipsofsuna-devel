local AiStateSpec = require("core/specs/aistate")

AiStateSpec{
	name = "bounce",
	calculate = function(self)
		for k,v in pairs(self.enemies) do return end
		return math.random()
	end,
	enter = function(self)
		-- Select target point.
		self:choose_wander_target()
	end,
	update = function(self, secs)
		if not self.target then return end
		-- Turn towards the target.
		self.object:face_point{point = self.target, secs = secs}
		-- Jump towards the target.
		self.object:set_movement(1)
		self.object:action("jump")
	end}
