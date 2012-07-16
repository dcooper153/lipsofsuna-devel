Ui:add_state{
	state = "newgame",
	root = "mainmenu",
	label = "New game",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
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
		return Widgets.Uibutton("Start", function()
			-- Prevent overwriting.
			local name = Operators.single_player:get_save_name()
			if #name == 0 then return end
			if Operators.single_player:does_save_name_exist(name) then return end
			-- Start the game.
			Settings.file = Operators.single_player:get_save_filename()
			Client:start_single_player()
		end)
	end}
