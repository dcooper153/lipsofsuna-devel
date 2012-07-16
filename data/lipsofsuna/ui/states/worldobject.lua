Ui:add_state{
	state = "world/object",
	label = "Interact",
	update = function()
		local object = Operators.world:get_manipulated_object()
		if not object or not object:get_visible() then
			Ui:pop_state()
		end
	end,
	init = function()
		local object = Operators.world:get_target_object()
		local usages = Operators.world:get_target_usages()
		if not object or not usages then
			Operators.world:set_manipulated_object(nil)
			return
		else
			Operators.world:set_manipulated_object(object)
		end
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
							Game.messaging:client_event("dialog start", object.id)
						else
							Client.active_dialog_object = object
							Ui.state = "dialog"
						end
					end))
				elseif v[1] == "loot" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						if not Operators.inventory:get_inventory_by_id(object.id) then
							Game.messaging:client_event("loot in world", object.id)
						end
						Client.data.inventory.id = object.id
						Ui.state = "loot"
					end))
				elseif v[1] == "pickpocket" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						-- FIXME: Should use a different system.
						if not Operators.inventory:get_inventory_by_id(object.id) then
							Game.messaging:client_event("pickpocket", object.id)
						end
						Client.data.inventory.id = object.id
						Ui.state = "loot"
					end))
				elseif v[1] == "pick up" then
					table.insert(widgets, Widgets.Uibutton(v[2], function()
						Game.messaging:client_event("take from world", object.id)
						Ui:pop_state()
					end))
				end
			end
		end
		return widgets
	end}
