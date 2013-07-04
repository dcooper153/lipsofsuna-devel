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
				count = item:get_count(),
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uiinvitem(object:get_id(), data, index, slot))
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "inventory/drop",
	label = "Drop stack"}

Ui:add_widget{
	state = "inventory/drop",
	widget = function()
		local count = Client.data.inventory.count
		return Widgets.Uiscrollinteger("Count", 1, count, count, function(w)
			Client.data.inventory.count = w.value
		end)
	end}

Ui:add_widget{
	state = "inventory/drop",
	widget = function()
		return Widgets.Uibutton("Drop", function()
			Main.messaging:client_event("drop from inventory", Client.data.inventory.index, Client.data.inventory.count)
			Ui:pop_state()
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "inventory/split",
	label = "Split stack"}

Ui:add_widget{
	state = "inventory/split",
	widget = function()
		local count = Client.data.inventory.count
		local half = math.max(1, math.ceil(count / 2))
		Client.data.inventory.count = half
		return Widgets.Uiscrollinteger("Count", 1, count-1, half, function(w)
			Client.data.inventory.count = w.value
		end)
	end}

Ui:add_widget{
	state = "inventory/split",
	widget = function()
		return Widgets.Uibutton("Split", function()
			Main.messaging:client_event("split in inventory", Client.data.inventory.id, Client.data.inventory.index, Client.data.inventory.count)
			Ui:pop_state()
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "inventory/move",
	history = "inventory",
	label = "Move item",
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
			table.insert(widgets, Widgets.Uiinvmove(object:get_id(), data, index, slot))
		end
		return widgets
	end}
