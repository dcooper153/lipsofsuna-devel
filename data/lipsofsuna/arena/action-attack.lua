Actionspec{
	name = "attack",
	categories =
	{
		["melee"] = true,
		["ranged"] = true,
		["throw"] = true
	},
	start = function(action)
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Check for cancel.
		if action.cancel then
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Check for charge start.
		if not action.charge_value then
			-- Check for release before the last cooldown ended.
			if action.object.cooldown then return true end
			if not action.object.control_right then return end
			-- Trigger special actions.
			local special = Main.combat_utils:get_combat_action_for_actor(action.object, "right")
			if special and special.name ~= "attack" then
				action.object:action(special.name)
				return
			end
			-- Find the finish action.
			local finish = Actionspec:find_by_name("melee")
			if not finish then return end
			-- Start the charge animation.
			local move = Main.combat_utils:get_melee_move_of_actor(action.object)
			action.object:animate("charge " .. move, true)
			-- Initialize the charge timer.
			action.finish_action = finish
			action.charge_move = move
			action.charge_value = 0
		end
		-- Check for charge finish.
		if not action.object.control_right then
			action.object.cooldown = nil
			action.object:action(action.finish_action.name, action.charge_move, action.charge_value)
			return
		end
		-- Continue charging.
		action.charge_value = action.charge_value + secs
		action.object.cooldown = 1
		return true
	end}
