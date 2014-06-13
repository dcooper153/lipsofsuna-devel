local UiTransition = require("ui/widgets/transition")

Ui:add_state{
	state = "menu",
	label = "Menu",
	root = "play"}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Inventory", "inventory") end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Crafting", "crafting",
		function() Operators.crafting:reset() end) end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Quests", "quests") end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Map", "map") end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Controls", "controls") end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Options", "options") end}

Ui:add_widget{
	state = "menu",
	widget = function()
		return UiTransition("Help", "help", function()
			Client.data.help.page = "index"
		end)
	end}

Ui:add_widget{
	state = "menu",
	widget = function() return UiTransition("Quit", "quit") end}
