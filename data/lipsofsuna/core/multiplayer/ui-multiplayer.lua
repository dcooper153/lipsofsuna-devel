local UiTransition = require("ui/widgets/transition")

Ui:add_state{
	state = "multiplayer",
	root = "mainmenu",
	label = "Multiplayer",
	background = function()
		return "mainmenu1"
	end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return UiTransition("Host", "host/new") end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return UiTransition("Load", "host/load") end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return UiTransition("Join", "join") end}
