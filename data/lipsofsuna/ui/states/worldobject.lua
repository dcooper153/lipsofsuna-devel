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
					Operators.world:use_object(object, action)
				end))
			else
				-- Add a special action.
				if v[1] == "dialog" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						if not object.dialog then
							Network:send{packet = Packet(packets.PLAYER_DIALOG, "uint32", object.id)}
						else
							Client.active_dialog_object = object
							Ui.state = "dialog"
						end
					end))
				elseif v[1] == "loot" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						if not Operators.inventory:get_inventory_by_id(object.id) then
							Network:send{packet = Packet(packets.PLAYER_LOOT_WORLD, "uint32", object.id)}
						end
						Client.data.inventory.id = object.id
						Ui.state = "loot"
					end))
				elseif v[1] == "pickpocket" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						-- FIXME: Should use a different system.
						if not Operators.inventory:get_inventory_by_id(object.id) then
							Network:send{packet = Packet(packets.PLAYER_PICKPOCKET, "uint32", object.id)}
						end
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
