Ui:add_state{
	state = "quit",
	label = "Quit"}

Ui:add_widget{
	state = "quit",
	widget = function() return Widgets.Uibutton("Quit to main menu", function()
			if Client.editor.initialized then
				Client.editor:uninitialize()
			end
			Client:terminate_game()
			Ui.state = "mainmenu"
		end)
	end}

Ui:add_widget{
	state = "quit",
	widget = function() return Widgets.Uibutton("Quit to desktop", function()
			Client:terminate_game()
			Program.quit = true
		end)
	end}
