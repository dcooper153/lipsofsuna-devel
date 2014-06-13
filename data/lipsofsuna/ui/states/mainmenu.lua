local UiBackground = require("ui/widgets/background")
local UiButton = require("ui/widgets/button")
local UiTransition = require("ui/widgets/transition")

Ui:add_state{
	state = "mainmenu",
	root = "mainmenu",
	label = "Main menu",
	background = function()
		return UiBackground("mainmenu1")
	end,
	init = function()
		Main.music_manager:clear_combat_hints()
		Main.music_manager:switch_track("menu")
	end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiTransition("New game", "newgame") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiTransition("Load game", "loadgame") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiTransition("Multiplayer", "multiplayer") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiTransition("Controls", "controls") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiTransition("Options", "options") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return UiButton("Quit", function() Program:set_quit(true) end) end}
