local UiButton = require("ui/widgets/button")

Ui:add_state{
	state = "quit",
	label = "Quit"}

Ui:add_widget{
	state = "quit",
	widget = function() return UiButton("Quit to main menu", function()
			if Client.editor and Client.editor.initialized then
				Client.editor:uninitialize()
			end
			Client:terminate_game()
			Ui:set_state("mainmenu")
		end)
	end}

Ui:add_widget{
	state = "quit",
	widget = function() return UiButton("Quit to desktop", function()
			Client:terminate_game()
			Program:set_quit(true)
		end)
	end}
