Actionspec{
	name = "block",
	start = function(action)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Trigger special actions.
		local special = Main.combat_utils:get_combat_action_for_actor(action.object, "left")
		if special and special.name ~= "block" and special.name ~= "block weapon" and special.name ~= "block shield" then
			action.object:action(special.name)
			return
		end
		-- Enable the blocking mode.
		action.object:set_block(true)
		return true
	end,
	update = function(action, secs)
		action.object.cooldown = action.object.spec.blocking_cooldown
		if action.cancel or not action.object.control_left then
			action.object:set_block(false)
			return
		end
		return true
	end}

Actionspec{
	name = "block weapon",
	start = function(action)
		action.object:action("block")
	end}

Actionspec{
	name = "block shield",
	start = function(action)
		action.object:action("block")
	end}
