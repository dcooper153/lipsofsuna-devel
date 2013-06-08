local Client = require("core/client/client")
local Keysym = require("system/keysym")
local Ui = require("ui/ui")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "menu", mode = "press", key1 = Keysym.TAB, func = function()
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

	Client.input:register_binding{name = "menu_up", mode = "toggle", key1 = "mousez+", func = function(v)
		Ui:command("up", v)
	end}

	Client.input:register_binding{name = "menu_down", mode = "toggle", key1 = "mousez-", func = function(v)
		Ui:command("down", v)
	end}

	Client.input:register_binding{name = "menu_back", mode = "toggle", key1 = Keysym.q, func = function(v)
		Ui:command("back", v)
	end}

	Client.input:register_binding{name = "menu_apply", mode = "toggle", key1 = Keysym.e, func = function(v)
		Ui:command("apply", v)
	end}

	Client.input:register_binding{name = "mouse_grab", mode = "press", key1 = Keysym.ESCAPE, func = function()
		-- Toggle mouse grabbing.
		local grab = not Ui:get_pointer_grab()
		Client.options.grab_cursor = grab
		Client.options:save()
		Ui:set_pointer_grab(grab)
		-- Open the in-game menu at ungrab.
		if not grab then
			if Ui:get_state() == "play" then
				Ui:set_state("menu")
			elseif Ui:get_state() == "editor" then
				Ui:set_state("editor/menu")
			end
		-- Close the in-game menu at grab.
		elseif Ui.root == "play" then
			Ui:set_state("play")
		end
	end}

end)
