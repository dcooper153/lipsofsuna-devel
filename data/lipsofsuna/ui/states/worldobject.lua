Ui:add_state{
	state = "world/object",
	label = "Interact",
	init = function()
		local object = Operators.world:get_target_object()
		local usages = Operators.world:get_target_usages()
		if not object or not usages then return end
		local widgets = {}
		for k,v in ipairs(usages) do
			local action = v[3]
			if action then
				-- Add a standard action.
				table.insert(widgets, Widgets.Uibutton(v[2], function()
					Network:send{packet = Packet(packets.PLAYER_USE_WORLD,
						"uint32", object.id,
						"string", action.name)}
					if action.dialog then
						Client.active_dialog_object = object
						Ui.state = "dialog"
					else
						Ui:pop_state()
					end
				end))
			else
				-- Add a special action.
				if v[1] == "dialog" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						if not object.dialog then
							Network:send{packet = Packet(packets.PLAYER_DIALOG, "uint32", object.id)}
						end
						Client.active_dialog_object = object
						Ui.state = "dialog"
					end))
				elseif v[1] == "loot" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						Network:send{packet = Packet(packets.PLAYER_LOOT_WORLD, "uint32", object.id)}
						Client.data.inventory.id = object.id
						Ui.state = "loot"
					end))
				elseif v[1] == "pickpocket" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						Network:send{packet = Packet(packets.PLAYER_PICKPOCKET, "uint32", object.id)}
						-- FIXME: Should use a different system.
						Client.data.inventory.id = object.id
						Ui.state = "loot"
					end))
				elseif v[1] == "pick up" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						Network:send{packet = Packet(packets.PLAYER_PICKUP, "uint32", object.id)}
						Ui:pop_state()
					end))
				end
			end
		end
		return widgets
	end}
