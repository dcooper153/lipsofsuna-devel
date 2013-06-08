local Client = require("core/client/client")
local Keysym = require("system/keysym")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "attack", mode = "toggle", key1 = "mouse1", func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("attack", v)
	end}

	Client.input:register_binding{name = "block", mode = "toggle", key1 = "mouse3", func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("block", v)
	end}

	Client.input:register_binding{name = "chat", mode = "press", key1 = Keysym.t, func = function()
		if not Client.player_object then return end
		Ui:set_state("chat")
	end}

	Client.input:register_binding{name = "climb", mode = "press", key1 = Keysym.c, func = function()
		if not Client.player_object then return end
		Game.messaging:client_event("climb")
	end}

	Client.input:register_binding{name = "feats", mode = "press", key1 = Keysym.u, func = function()
		if Client.player_object then
			if Ui:get_state() ~= "feats" then
				Ui:set_state("feats")
			else
				Ui:set_state("play")
			end
		elseif Ui.root == "editor" then
			-- Editor controls.
			if Client.editor.prev_tiles[1] and Client.editor.prev_tiles[2] then
				Client.editor:fill(Client.editor.prev_tiles[1], Client.editor.prev_tiles[2])
			end
		end
	end}

	Client.input:register_binding{name = "inventory", mode = "press", key1 = Keysym.i, func = function()
		if not Client.player_object then return end
		if Ui:get_state() ~= "inventory" then
			Ui:set_state("inventory")
		else
			Ui:set_state("play")
		end
	end}

	Client.input:register_binding{name = "jump", mode = "toggle", key1 = Keysym.SPACE, func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("jump", v)
	end}

	Client.input:register_binding{name = "map", mode = "press", key1 = Keysym.m, func = function()
		if not Client.player_object then return end
		if Ui:get_state() ~= "map" then
			Ui:set_state("map")
		else
			Ui:set_state("play")
		end
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

	Client.input:register_binding{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("walk", math.max(-1, math.min(1, -v)))
	end}

	Client.input:register_binding{name = "options", mode = "press", key1 = Keysym.o, func = function()
		if not Client.player_object then return end
		if Ui:get_state() ~= "options" then
			Ui:set_state("options")
		else
			Ui:set_state("play")
		end
	end}

	Client.input:register_binding{name = "pick_up", mode = "press", key1 = Keysym.COMMA, func = function()
		if not Client.player_object then return end
		-- TODO
	end}

	Client.input:register_binding{name = "quests", mode = "press", key1 = Keysym.n, func = function()
		if not Client.player_object then return end
		if Ui:get_state() ~= "quests" then
			Ui:set_state("quests")
		else
			Ui:set_state("play")
		end
	end}

	Client.input:register_binding{name = "run", mode = "toggle", key1 = Keysym.LSHIFT, func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("run", not v)
	end}

	Client.input:register_binding{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
		local n = Program:capture_screen()
		Client:append_log("Screenshot: " .. n)
	end}

	Client.input:register_binding{name = "skills", mode = "press", key1 = Keysym.k, func = function()
		if not Client.player_object then return end
		if Ui:get_state() ~= "skills" then
			Ui:set_state("skills")
		else
			Ui:set_state("play")
		end
	end}

	Client.input:register_binding{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
		if not Client.player_object then return end
		Game.messaging:client_event("sidestep", v)
	end}

	Client.input:register_binding{name = "use", mode = "press", key1 = Keysym.b, func = function()
		if not Client.player_object then return end
		Ui:set_state("world/object")
	end}

end)
