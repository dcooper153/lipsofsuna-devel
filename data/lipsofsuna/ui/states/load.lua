local UiBackground = require("ui/widgets/background")

Ui:add_state{
	state = "load",
	root = "load",
	label = "Loading",
	hint = "Loading the map, please wait.",
	background = function()
		return UiBackground("mainmenu1")
	end,
	init = function()
		Client.data.load.timer = 0
		Client.data.load.finished = false
	end,
	update = function(secs)
		if Client.data.load.finished then
			Ui:set_state(Client.data.load.next_state)
			return
		end
		if Client.data.load.timer >= 0.1 then
			Client:create_world()
			Client.data.load.finished = true
		end
		Client.data.load.timer = Client.data.load.timer + secs
	end}
