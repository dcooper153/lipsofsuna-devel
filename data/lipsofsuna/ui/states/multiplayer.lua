Ui:add_state{
	state = "multiplayer",
	root = "mainmenu",
	label = "Multiplayer",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return Widgets.Uitransition("Host", "host/new") end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return Widgets.Uitransition("Load", "host/load") end}

Ui:add_widget{
	state = "multiplayer",
	widget = function() return Widgets.Uitransition("Join", "join") end}
