Aistatespec{
	name = "combat",
	calculate = function(self)
		-- Check for enemies.
		for k,v in pairs(self.enemies) do return 1 end
	end,
	enter = function(self)
		self:calculate_combat_ratings()
		self.target = self.best_enemy
	end,
	update = function(self, secs)
		-- Check if the target is has died.
		if not self.target or self.target.dead then
			self.ai_timer = self.object.spec.ai_update_delay
			return
		end
		-- Decide what combat action to perform next.
		if self.action_timer <= 0 then
			self:choose_combat_action()
		end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target.position, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
	end}
