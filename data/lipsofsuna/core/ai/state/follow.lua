local AiStateSpec = require("core/specs/aistate")

AiStateSpec{
	name = "follow",
	calculate = function(self)
		for k,v in pairs(self.enemies) do return end
		if not self.object.owner then return end
		if not self.object.owner:get_visible() then return end
		return 2
	end,
	update = function(self, secs)
		if not self.object.owner then return end
		if not self.object.owner:get_visible() then return end
		local target = self.object.owner:get_position()
		-- Turn towards the target.
		local face = self.object:face_point{point = target, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Move towards the target.
		local dist = (target - self.object:get_position()).length
		if dist < 2 then
			self.object:set_movement(0)
		else
			self.object:set_movement(1)
		end
		-- Toggle running based on the distance.
		if self.object:get_running() then
			if dist < 3 then
				self.object:set_running(false)
			end
		else
			if dist > 3.5 then
				self.object:set_running(true)
			end
		end
	end}
