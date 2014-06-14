local ActionSpec = require("core/specs/action")

ActionSpec{
	name = "examine",
	label = "Examine",
	start = function(action, object)
		if not Main.dialogs then return end
		if Main.dialogs:execute(object, action.object) then
			Main.messaging:server_event("object dialog", action.object.client, object:get_id())
		end
	end}
