Actionspec{
	name = "resurrect",
	label = "Resurrect",
	start = function(action)
		local self = action.object
		if not self.dead then return end
		-- Enable controls.
		self.dead = nil
		self:set_beheaded(false)
		self.physics:set_shape("default")
		self.physics:set_physics("kinematic")
		-- Enable stats.
		self.stats.enabled = true
		self.stats:set_value("health", 1)
		-- Restore the idle animation.
		self:animate("idle")
		-- Emit a vision event.
		Server:object_event(self, "object-dead", {dead = false})
	end}
