local Network = require("system/network")
local UiButton = require("ui/widgets/button")

Ui:add_state{
	state = "host/wait",
	root = "host/wait",
	label = "Launching",
	update = function(secs)
		if Main.host:update(secs) then
			Ui:restart_state()
		end
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return UiButton("Retry", function()
			Client:terminate_game()
			Main.host:host_game()
			--Client:start_single_player()
		end)
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return UiButton("Cancel", function()
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "host/wait",
	widget = function() return Widgets.Uilabel(Main.host:get_status()) end}
