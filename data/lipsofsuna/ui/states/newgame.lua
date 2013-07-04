local UiSelector = require("ui/widgets/selector")

local game_mode = "Normal"

Ui:add_state{
	state = "newgame",
	root = "mainmenu",
	label = "New game",
	background = function()
		return Widgets.Uibackground("mainmenu1")
	end}

Ui:add_widget{
	state = "newgame",
	widget = function()
		local widget = Widgets.Uientry("Save name", function(w)
			Operators.single_player:set_save_name(w.value) end)
		widget.value = Operators.single_player:generate_new_save_name()
		return widget
	end}

Ui:add_widget{
	state = "newgame",
	widget = function()
		-- Get the items from main.
		local modes = {}
		for k,v in Main.game_modes:get_modes() do
			table.insert(modes, {k, function(w) game_mode = k end})
		end
		-- Sort the items.
		table.sort(modes, function(a, b) return a[1] < b[1] end)
		-- Find the default item.
		local mode
		for k,v in ipairs(modes) do
			if v[1] == game_mode then
				mode = k
				break
			end
		end
		if not mode then
			mode = 1
			game_mode = modes[mode]
		end
		-- Create the selector widget.
		local widget = UiSelector("Game mode", modes)
		widget:select(mode)
		return widget
	end}

Ui:add_widget{
	state = "newgame",
	widget = function()
		return Widgets.Uibutton("Start", function()
			-- Prevent overwriting.
			local name = Operators.single_player:get_save_name()
			if #name == 0 then return end
			if Operators.single_player:does_save_name_exist(name) then return end
			-- Start the game.
			Settings.file = Operators.single_player:get_save_filename()
			Main:start_game(game_mode)
		end)
	end}
