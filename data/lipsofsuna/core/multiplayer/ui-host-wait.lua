local Network = require("system/network")

Ui:add_state{
	state = "host/wait",
	root = "host/wait",
	label = "Launching",
	grab = function()
		return false
	end,
	update = function(secs)
		if Client.host:update(secs) then
			Ui:restart_state()
		end
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return Widgets.Uibutton("Retry", function()
			Client:terminate_game()
			Client.host:host_game()
			--Client:start_single_player()
		end)
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return Widgets.Uibutton("Cancel", function()
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return Widgets.Uilabel(Client.host:get_status()) end}
