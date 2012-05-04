Ui:add_state{
	state = "world/object",
	label = "Interact",
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
			if spec.inventory_size then
				table.insert(widgets, Widgets.Uibutton("Loot", function()
					Network:send{packet = Packet(packets.PLAYER_LOOT_WORLD, "uint32", object.id)}
					Client.data.inventory.id = object.id
					Ui.state = "loot"
				end))
			end
			table.insert(widgets, Widgets.Uibutton("Pick up", function()
				Network:send{packet = Packet(packets.PLAYER_PICKUP, "uint32", object.id)}
				Ui:pop_state()
			end))
		end
		-- Create action widgets.
		if spec.get_use_actions then
			for k,v in pairs(spec:get_use_actions()) do
				local name = v.label or v.name
				local action = v.name
				table.insert(widgets, Widgets.Uibutton(name, function()
					Network:send{packet = Packet(packets.PLAYER_USE_WORLD,
						"uint32", object.id,
						"string", v.name)}
					if v.dialog then
						Client.active_dialog_object = object
						Ui.state = "dialog"
					else
						Ui:pop_state()
					end
				end))
			end
		end
		return widgets
	end}
