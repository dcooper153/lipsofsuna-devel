local Client = require("core/client/client")
local Network = require("system/network")
local Ui = require("ui/ui")

Ui:add_state{
	state = "join/connect",
	root = "join/connect",
	label = "Connecting",
	update = function(secs)
		if Main.join:update(secs) then
			Ui:restart_state()
		end
	end}

Ui:add_widget{
	state = "join/connect",
	widget = function() return Widgets.Uibutton("Retry", function()
			Client:terminate_game()
			Main.join:join_game()
		end)
	end}

Ui:add_widget{
	state = "join/connect",
	widget = function() return Widgets.Uibutton("Cancel", function()
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "join/connect",
	widget = function() return Widgets.Uilabel(Main.join:get_status()) end}
