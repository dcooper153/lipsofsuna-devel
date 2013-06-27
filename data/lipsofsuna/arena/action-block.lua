Actionspec{
	name = "block",
	start = function(action)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Trigger special weapon actions.
		local weapon = action.object:get_weapon()
		if weapon then
			local name = weapon.spec.actions["right"]
			if name then
				action.object:action(name)
				return
			end
		end
		-- Enable the blocking mode.
		action.object:set_block(true)
		return true
	end,
	update = function(action, secs)
		action.object.cooldown = 0.3
		if action.cancel or action.finish then
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
