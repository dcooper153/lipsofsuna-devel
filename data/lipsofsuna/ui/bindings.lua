local Client = require("core/client/client")
local Keysym = require("system/keysym")
local Ui = require("ui/ui")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
		if Ui.root == "play" then
			-- Game controls.
			if Ui:get_state() ~= "play" then
				Ui:set_state("play")
			else
				Ui:set_state("menu")
			end
		elseif Ui.root == "editor" then
			-- Editor controls.
			if Ui:get_state() ~= "editor" then
				Ui:set_state("editor")
			else
				Ui:set_state("editor/menu")
			end
		elseif Ui.root == "chargen" then
			Ui:set_state("chargen")
		elseif Ui.root == "start-game" then
			Ui:set_state("start-game")
		else
			-- Main menu controls.
			Ui:set_state("mainmenu")
		end
	end}

	Client.input:register_binding{name = "menu_up", mode = "toggle", key1 = Keysym.UP, func = function(v)
		Ui:command("up", v)
	end}

	Client.input:register_binding{name = "menu_down", mode = "toggle", key1 = Keysym.DOWN, func = function(v)
		Ui:command("down", v)
	end}

	Client.input:register_binding{name = "menu_left", mode = "toggle", key1 = Keysym.LEFT, func = function(v)
		Ui:command("left", v)
	end}

	Client.input:register_binding{name = "menu_right", mode = "toggle", key1 = Keysym.RIGHT, func = function(v)
		Ui:command("right", v)
	end}

	Client.input:register_binding{name = "menu_back", mode = "toggle", key1 = Keysym.BACKSPACE, func = function(v)
		Ui:command("back", v)
	end}

	Client.input:register_binding{name = "menu_apply", mode = "toggle", key1 = Keysym.ENTER, func = function(v)
		Ui:command("apply", v)
	end}

end)
