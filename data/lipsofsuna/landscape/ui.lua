local Ui = require("ui/ui")

Ui:add_state{
	state = "landscape",
	root = "landscape",
	exit_root = function()
		Main:end_game()
		Ui:set_state("mainmenu")
	end,
	grab = function()
		return true
	end,
	init = function()
		Main.music_manager:switch_track("game")
	end}
