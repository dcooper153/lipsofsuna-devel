Ui:add_state{
	state = "play",
	root = "play",
	hint = "",
	init = function()
		Sound:switch_music_track("game")
		if Client.camera ~= Client.camera1 and Client.camera ~= Client.camera2 then
			Client.camera = Client.camera3
		end
	end,
	input = function(args)
		-- Only handle apply button presses.
		if args.type ~= "keypress" then return true end
		local action = Action.dict_name["menu apply"]
		if not action then return true end
		if not action:handle_event(args) then return true end
		-- Activate the item the player is looking at.
		if Target.target_object then
			Ui.state = "world/object"
		end
	end,
	update = function()
		if not Client.player_object then return end
		if Client.player_object.dead then
			Ui.state = "respawn"
		elseif not Program.cursor_grabbed then
			Program.cursor_grabbed = true
		end
	end}
