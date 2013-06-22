Actionspec{
	name = "jump",
	label = "Jump",
	start = function(action)
		-- Check for preconditions.
		local self = action.object
		if self.blocking then return end
		if self.climbing then return end
		local t = Program:get_time()
		if t - self.jumped < 0.5 then return end
		-- Jump or swim.
		if self.submerged and self.submerged > 0.4 then
			-- Swimming upwards.
			if self:get_burdened() then return end
			local v = self:get_velocity()
			self.jumped = t - 0.3
			self.jumping = true
			if v.y < self.physics:get_speed() then
				self.physics:jump(Vector(v.x, self.spec.swim_force * self.spec.mass, v.z))
			end
		else
			-- Jumping.
			if not self.physics:get_ground() or self:get_burdened() then return end
			self.jumped = t
			self.jumping = true
			Server:object_effect(self, "jump1")
			self:animate("jump")
			local v = self:get_velocity()
			local f = self.spec.mass * self.spec.jump_force * self.attributes.jump
			self.physics:jump(Vector(v.x, f, v.z))
		end
	end}
