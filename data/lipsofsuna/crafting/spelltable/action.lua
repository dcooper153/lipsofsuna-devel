Actionspec{
	name = "craft spells",
	label = "Craft spells",
	start = function(action, item)
		if not item then return end
		action.strength = 1
		action.device = item
		Main.messaging:server_event("enchant", action.object.client, true)
		return true
	end,
	update = function(action, secs)
		-- Check that the player is alive.
		if action.object.dead or not action.object:get_visible() then
			Main.messaging:server_event("enchant", action.object.client, false)
			return
		end
		-- Check that the crafting device is visible.
		if not action.device:get_visible() then
			Main.messaging:server_event("enchant", action.object.client, false)
			return
		end
		-- Check that the crafting device is nearby.
		local dist = (action.object:get_position() - action.device:get_position()).length
		if dist > 10 then
			Main.messaging:server_event("enchant", action.object.client, false)
			return
		end
		-- Continue enchanting.
		return true
	end}
