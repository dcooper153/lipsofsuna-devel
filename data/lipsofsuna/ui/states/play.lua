Ui:add_state{
	state = "play",
	root = "play",
	hint = "",
	init = function()
		Client.effects:switch_music_track("game")
		Client.camera_manager:set_camera_mode("third-person")
	end,
	input = function(args)
		-- Only handle apply button presses.
		if args.type ~= "keypress" then return true end
		local action = Client.bindings:find_by_name("menu apply")
		if not action then return true end
		if not action:handle_event(args) then return true end
		-- Activate the item the player is looking at.
		if Operators.world:get_target_object() then
			Ui:set_state("world/object")
		end
	end,
	update = function()
		if not Client.player_object then return end
		if Client.player_object.dead then
			Ui:set_state("respawn")
		elseif not Ui:get_pointer_grab() then
			Ui:set_pointer_grab(true)
		end
	end}
