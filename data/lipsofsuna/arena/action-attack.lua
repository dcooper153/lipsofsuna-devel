local ActionSpec = require("core/specs/action")

ActionSpec{
	name = "attack",
	categories =
	{
		["melee"] = true,
		["ranged"] = true,
		["throw"] = true
	},
	start = function(action)
		-- Let the AI attack immediately.
		if action.object.ai then
			if action.object.cooldown then return end
			local chained = Main.combat_utils:get_combat_action_for_actor(action.object, "right")
			if not chained or chained.name == "attack" then
				chained = Main.specs:find_by_name("ActionSpec", "melee")
				if not chained then return end
			end
			action.object:action(chained.name)
			return
		end
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
			local finish = Main.specs:find_by_name("ActionSpec", "melee")
			if not finish then return end
			-- Start the charge animation.
			local move = Main.combat_utils:get_melee_move_of_actor(action.object)
			action.object:animate("charge stand", true)
			-- Initialize the charge timer.
			action.finish_action = finish
			action.charge_move = move
			action.charge_value = 0
		end
		-- Check for charge finish.
		if not action.object.control_right then
			-- Directional combat arts.
			if action.charge_value < 0.5 then
				local a = action.object.skills:get_combat_art(action.charge_move)
				if a then
					action.object.cooldown = nil
					action.object:action(a.name)
					return
				end
			end
			-- Normal melee attacks.
			action.object.cooldown = nil
			action.object:action(action.finish_action.name, "stand", action.charge_value)
			return
		end
		-- Continue charging.
		action.charge_value = action.charge_value + secs
		action.object.cooldown = 1
		return true
	end,
	get_score = function(action)
		return 1
	end}
