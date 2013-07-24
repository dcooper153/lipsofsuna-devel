Ui:add_state{
	state = "mainmenu",
	root = "mainmenu",
	label = "Main menu",
	background = function()
		return Widgets.Uibackground("mainmenu1")
	end,
	init = function()
		Client.music:clear_combat_hints()
		Client.music:switch_track("menu")
	end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("New game", "newgame") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Load game", "loadgame") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Multiplayer", "multiplayer") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Controls", "controls") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Options", "options") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function()
		local widget = Widgets.Uibutton("Editor", function() Ui:set_state("editor") end)
		widget.hint = "$A: Enter\n$$B\n$$U\n$$D"
		return widget
	end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uibutton("Quit", function() Program:set_quit(true) end) end}
