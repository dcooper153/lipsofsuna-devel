local ActionSpec = require("core/specs/action")

ActionSpec{
	name = "cancel ranged",
	start = function(action)
		local ranged = action.object:find_action_by_name("ranged")
		if ranged then
			ranged.cancel = true
		end
	end}
