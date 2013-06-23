Ui:add_state{
	state = "load",
	root = "load",
	label = "Loading",
	hint = "Loading the map, please wait.",
	background = function()
		return Widgets.Uibackground("mainmenu1")
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
			Map:init()
			if Server.initialized then
				Server:load()
			end
			Client.data.load.finished = true
		end
		Client.data.load.timer = Client.data.load.timer + secs
	end}
