Aiactionspec{
	name = "bounce",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self)
		return math.random()
	end,
	perform = function(self, secs)
		if not self.target then return end
		-- Turn towards the target.
		self.object:face_point{point = self.target:get_position(), secs = secs}
		-- Jump towards the target.
		self.object:set_movement(1)
		self.object:jump()
	end}
