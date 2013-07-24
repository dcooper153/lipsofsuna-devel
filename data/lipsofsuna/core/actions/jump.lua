Actionspec{
	name = "jump",
	label = "Jump",
	start = function(action)
		-- Check for preconditions.
		local self = action.object
		if self.blocking then return end
		if self.climbing then return end
		-- Jump or swim.
		if self.submerged and self.submerged > 0.4 then
			-- Swimming upwards.
			if self:get_burdened() then return end
			local v = self:get_velocity()
			self.jumping = true
			if v.y < self.physics:get_speed() then
				self.physics:jump(Vector(v.x, self.spec.swim_force * self.spec.mass, v.z))
			end
		else
			-- Jumping.
			if not self.physics:get_ground() or self:get_burdened() then return end
			self.jumping = true
			Main.vision:object_effect(self, "jump1")
			self:animate("jump")
			local v = self:get_velocity()
			local f = self.spec.mass * self.spec.jump_force * self.attributes.jump
			self.physics:jump(Vector(v.x, f, v.z))
			-- Enable effect-over-time updates.
			action.timer = 0
			return true
		end
	end,
	update = function(action, secs)
		-- Cancellation.
		local self = action.object
		if not self.jumping then return end
		if self.dead then return end
		-- Auto-climbing for NPCs.
		if not self.client then
			self:climb()
		end
		-- Wait for landing.
		action.timer = action.timer + secs
		if action.timer < 0.1 then return true end
		if not self.physics:get_ground() then return true end
		-- Landing animation.
		if not self.submerged or self.submerged < 0.3 then
			self:animate("land ground")
			Main.vision:object_effect(self, self.spec.effect_landing)
		else
			self:animate("land water")
		end
		self.jumping = nil
	end}
