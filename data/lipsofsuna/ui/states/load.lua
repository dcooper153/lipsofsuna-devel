Ui:add_state{
	state = "load",
	root = "load",
	label = "Loading",
	hint = "Loading the map, please wait.",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end,
	init = function()
		Client.data.load.timer = 0
	end,
	update = function(secs)
		if not Map or Map.initialized then
			Ui.state = Client.data.load.next_state
		end
		if Client.data.load.timer >= 0.1 then
			Client:create_world()
			Map:init()
		end
		Client.data.load.timer = Client.data.load.timer + secs
	end}
