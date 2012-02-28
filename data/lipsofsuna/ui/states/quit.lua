
Ui:add_widget{
	state = "quit",
	widget = function() return Widgets.Uibutton("Quit to main menu", function()
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
