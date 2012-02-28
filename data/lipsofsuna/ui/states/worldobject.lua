
Ui:add_state{
	state = "world/object",
	init = function()
		-- Get the object.
		local object = Target.target_object
		if not object then return end
		if not object.realized then return end
		-- Get the object spec.
		local spec = object.spec
		if not spec then return end
		-- Create the widgets.
		local widgets = {}
		if spec.type == "species" then
			if spec.dialog then
				table.insert(widgets, Widgets.Uibutton(object.dead and "Evaluate" or "Chat", function()
					if not object.dialog then
						Network:send{packet = Packet(packets.PLAYER_DIALOG, "uint32", object.id)}
					end
					Client.active_dialog_object = object
					Ui.state = "dialog"
				end))
			end
			if object.dead then
				table.insert(widgets, Widgets.Uibutton("Loot", function()
					Network:send{packet = Packet(packets.PLAYER_LOOT_WORLD, "uint32", object.id)}
					Client.data.inventory.id = object.id
					Ui.state = "loot"
				end))
			else
				table.insert(widgets, Widgets.Uibutton("Pickpocket", function()
					Network:send{packet = Packet(packets.PLAYER_PICKPOCKET, "uint32", object.id)}
					-- FIXME: Should use a different system.
					Client.data.inventory.id = object.id
					Ui.state = "loot"
				end))
			end
		elseif spec.type == "item" then
			table.insert(widgets, Widgets.Uibutton("Pick up", function()
				Network:send{packet = Packet(packets.PLAYER_PICKUP, "uint32", object.id)}
				Ui:pop_state()
			end))
			if spec.inventory_size then
				table.insert(widgets, Widgets.Uibutton("Loot", function()
					Network:send{packet = Packet(packets.PLAYER_LOOT_WORLD, "uint32", object.id)}
					Client.data.inventory.id = object.id
					Ui.state = "loot"
				end))
			end
		elseif spec.type == "obstacle" then
			if spec.harvest_enabled then
				table.insert(widgets, Widgets.Uibutton("Harvest", function()
					Network:send{packet = Packet(packets.PLAYER_HARVEST, "uint32", object.id)}
					Ui:pop_state()
				end))
			end
			if spec.interactive then
				table.insert(widgets, Widgets.Uibutton("Use", function()
					Network:send{packet = Packet(packets.PLAYER_USE_WORLD, "uint32", object.id)}
					Ui:pop_state()
				end))
			end
		end
		return widgets
	end}
