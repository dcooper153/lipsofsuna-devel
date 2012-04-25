Aistatespec{
	name = "wait",
	calculate = function(self)
		-- Check if the allowed for the actor.
		if not self.object.spec.ai_wait_allowed then return end
		-- Start waiting after wandering for a while.
		if self.state ~= "wander" then return end
		if self.state_timer < self.object.spec.ai_wander_time then return end
		return 1
	end,
	update = function(self, secs)
		self.object:set_movement(0)
	end}
