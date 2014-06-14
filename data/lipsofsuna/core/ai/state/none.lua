local AiStateSpec = require("core/specs/aistate")

AiStateSpec{
	name = "none",
	calculate = function(self)
		return 0.00000001
	end,
	update = function(self, secs)
	end}
