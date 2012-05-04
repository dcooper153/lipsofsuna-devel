Ui:add_state{
	state = "inventory",
	label = "Inventory",
	init = function()
		local object = Client.player_object
		if not object then return end
		local widgets = {}
		for index = 1,object.inventory.size do
			local item = object.inventory:get_object_by_index(index)
			local slot = object.inventory:get_slot_by_index(index)
			local data = item and {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uiinvitem(object.id, data, index, slot))
		end
		return widgets
	end,
	input = function(args)
		if not Drag.drag then return true end
		if args.type ~= "mousemotion" then return true end
		if args.rel > 0 then
			Drag:change_count(1)
		else
			Drag:change_count(-1)
		end
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "inventory/container",
	init = function()
		-- Get the active container.
		if not Client.data.inventory.id then return end
		local object = Object:find{id = Client.data.inventory.id}
		if not object then return end
		-- Create the inventory slots.
		local widgets = {}
		for index = 1,object.inventory.size do
			local item = object.inventory:get_object_by_index(index)
			local data = item and {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uiinvitem(object.id, index, data))
		end
		return widgets
	end,
	input = function(args)
		if not Drag.drag then return true end
		if args.type ~= "mousemotion" then return true end
		if args.rel > 0 then
			Drag:change_count(1)
		else
			Drag:change_count(-1)
		end
	end}

------------------------------------------------------------------------------

Ui:add_widget{
	state = "inventory/item",
	widget = function()
		-- Get the active container.
		if not Client.data.inventory.id then return end
		local object = Object:find{id = Client.data.inventory.id}
		if not object then return end
		-- Get the active item.
		local index = Client.data.inventory.index
		local item = object.inventory:get_object_by_index(index)
		if not item then return end
		-- Get the equipment slot.
		local slot = item.spec.equipment_slot
		if not slot then return end
		-- Don't show the widget if the item is already equipped.
		if object.inventory:get_slot_by_index(index) then return end
		-- Create the widget.
		return Widgets.Uibutton("Equip in " .. slot, function()
			Network:send{packet = Packet(packets.PLAYER_EQUIP, "uint32", index, "string", slot)}
			Ui:pop_state()
		end)
	end}

Ui:add_widget{
	state = "inventory/item",
	widget = function()
		-- Get the active container.
		local object = Object:find{id = Client.data.inventory.id}
		if not object then return end
		-- Don't show the widget if the item isn't equipped.
		local index = Client.data.inventory.index
		if not object.inventory:get_slot_by_index(index) then return end
		-- Create the widget.
		return Widgets.Uibutton("Unequip", function()
			Network:send{packet = Packet(packets.PLAYER_UNEQUIP, "uint32", index)}
			Ui:pop_state()
		end)
	end}

Ui:add_widget{
	state = "inventory/item",
	widget = function()
		-- Get the active container.
		if not Client.data.inventory.id then return end
		local object = Object:find{id = Client.data.inventory.id}
		if not object then return end
		-- Get the active item.
		local index = Client.data.inventory.index
		local item = object.inventory:get_object_by_index(index)
		if not item then return end
		-- Add a widget for each special usage.
		local widgets = {}
		for k,v in pairs(item.spec:get_use_actions()) do
			local name = v.label or v.name
			local action = v.name
			table.insert(widgets, Widgets.Uibutton(name, function()
				Network:send{packet = Packet(packets.PLAYER_USE_INVENTORY,
					"uint32", Client.data.inventory.id,
					"uint32", Client.data.inventory.index,
					"string", action)}
				Ui:pop_state()
			end))
		end
		return widgets
	end}

Ui:add_widget{
	state = "inventory/item",
	widget = function() return Widgets.Uibutton("Drop", function()
			Network:send{packet = Packet(packets.PLAYER_DROP, "uint32", Client.data.inventory.index, "uint32", Client.data.inventory.count)}
			Ui:pop_state()
		end)
	end}

Ui:add_widget{
	state = "inventory/item",
	widget = function()
		-- Don't show the widget if the count isn't at least two.
		local count = Client.data.inventory.count
		if count < 2 then return end
		-- Create the widget.
		return Widgets.Uiscrollinteger("Count", 1, count, count, function(w)
			Client.data.inventory.count = w.value
		end)
	end}
