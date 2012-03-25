Ui:add_state{
	state = "trading",
	label = "Trading"}

Ui:add_widget{
	state = "trading",
	widget = function() return Widgets.Uilabel("Goods") end}

Ui:add_widget{
	state = "trading",
	widget = function()
		local object = Client.player_object
		if not object then return end
		local widgets = {}
		for index = 1,5 do
			local shopidx = Client.data.trading.buy[index]
			local item = shopidx and Client.data.trading.shop[shopidx]
			local data = item and {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uitradeslot(data, index, nil, true))
		end
		return widgets
	end}

Ui:add_widget{
	state = "trading",
	widget = function() return Widgets.Uilabel("Payment") end}

Ui:add_widget{
	state = "trading",
	widget = function()
		local object = Client.player_object
		if not object then return end
		local widgets = {}
		for index = 1,5 do
			local invidx = Client.data.trading.sell[index]
			local item = invidx and object.inventory:get_object_by_index(invidx)
			local slot = invidx and object.inventory:get_slot_by_index(invidx)
			local data = item and {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uitradeslot(data, index, slot, false))
		end
		return widgets
	end}

Ui:add_widget{
	state = "trading",
	widget = function()
		if Client.data.trading.accepted then
			return Widgets.Uibutton("Accept", function() Client:apply_trade() end)
		else
			return Widgets.Uilabel("You need to pay more.")
		end
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "trading/buy",
	init = function()
		local object = Client.player_object
		if not object then return end
		local widgets = {}
		for index,item in ipairs(Client.data.trading.shop) do
			local data = {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uitradeitem(data, index, nil, true))
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "trading/sell",
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
			table.insert(widgets, Widgets.Uitradeitem(data, index, slot, false))
		end
		return widgets
	end}
