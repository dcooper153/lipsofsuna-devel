Actionspec{
	name = "say",
	label = "Say",
	start = function(action, msg)
		if not msg then return end
		Server:object_event(action.object, "object-speech", {message = msg})
	end}
