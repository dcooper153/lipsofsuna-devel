Ui:add_state{
	state = "menu",
	label = "Menu",
	root = "play"}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Inventory", "inventory") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Crafting", "crafting",
		function() Operators.crafting:reset() end) end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Quests", "quests") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Skills", "skills") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Spells", "spells") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Map", "map") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Controls", "controls") end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Options", "options") end}

Ui:add_widget{
	state = "menu",
	widget = function()
		return Widgets.Uitransition("Help", "help", function()
			Client.data.help.page = "index"
		end)
	end}

Ui:add_widget{
	state = "menu",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}
