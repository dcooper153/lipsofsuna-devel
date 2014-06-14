local ActionSpec = require("core/specs/action")

ActionSpec{
	name = "resurrect",
	label = "Resurrect",
	start = function(action)
		action.object:set_dead(false)
	end}
