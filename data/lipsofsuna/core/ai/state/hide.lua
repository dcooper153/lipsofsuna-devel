local AiStateSpec = require("core/specs/aistate")

AiStateSpec{
	name = "hide",
	calculate = function(self)
		-- Hide after fleeing.
		if self.state ~= "hide" then return end
		if self.state_timer < 20 then return end
		-- TODO
		-- return 1
	end,
	update = function(self, secs)
		-- TODO
	end}
