local ActionSpec = require("core/specs/action")

ActionSpec{
	name = "jump stop",
	label = "Jump stop",
	start = function(action)
		local self = action.object
		if not self.jumping then return end
		if self.submerged then return end
		local init_y = self.spec.jump_force * self.spec.mass
		local vel = self:get_velocity()
		if vel.y > 0 and vel.y < init_y then
			vel.y = 0
			self:set_velocity(vel)
		end
	end}
