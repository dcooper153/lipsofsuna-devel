local choose_move = function(attacker)
	if attacker:get_strafing() < -0.2 then return "left"
	elseif attacker:get_strafing() > 0.2 then return "right"
	elseif attacker:get_movement() < -0.2 then return "back"
	elseif attacker:get_movement() > 0.2 then return "front"
	else return "stand" end
end

--[[
Actionspec{
	name = "block",
	start = function(action)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Get the action name.
		local name = left and "block" or "attack"
		local weapon = self:get_weapon()
		if weapon then
			name = weapon.spec.actions[left and "left" or "right"]
			if not name then return end
		end
]]

Actionspec{
	name = "attack",
	start = function(action)
		-- Prevent during cooldown.
		if action.object.cooldown then return end
		-- Trigger special weapon actions.
		local weapon = action.object:get_weapon()
		if weapon then
			name = weapon.spec.actions["right"]
			if name and name ~= "attack" then
				action.object:action(name)
				return
			end
		end
		-- Find the finish action.
		local finish = Actionspec:find_by_name("melee")
		if not finish then return end
		-- Start the charge animation.
		local move = choose_move(action.object)
		action.object:animate("charge " .. move, true)
		-- Enable effect-over-time updates.
		action.finish_action = finish
		action.charge_move = move
		action.charge_value = 0
		return true
	end,
	update = function(action, secs)
		-- Check for cancel.
		if action.cancel then
			action.object.cooldown = 0.4
			action.object:animate("charge cancel")
			return
		end
		-- Check for charge finish.
		if action.finish then
			action.object.cooldown = 0.8
			action.object:action(action.finish_action.name, action.charge_move, action.charge_value)
			return
		end
		-- Continue charging.
		action.charge_value = action.charge_value + secs
		action.object.cooldown = 1
		return true
	end}
