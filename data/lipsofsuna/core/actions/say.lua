Actionspec{
	name = "say",
	label = "Say",
	start = function(action, msg)
		if not msg then return end
		Main.vision:object_event(action.object, "object-speech", {message = msg})
	end}
