Ui:add_state{
	state = "trading",
	label = "Trading"}

Ui:add_widget{
	state = "trading",
	widget = function() return Widgets.Uilabel("Goods") end}

Ui:add_widget{
	state = "trading",
	widget = function()
		local items = Operators.trading:get_buy_items()
		local widgets = {}
		for index = 1,5 do
			table.insert(widgets, Widgets.Uitradeslot(items[index], index, true))
		end
		return widgets
	end}

Ui:add_widget{
	state = "trading",
	widget = function() return Widgets.Uilabel("Payment") end}

Ui:add_widget{
	state = "trading",
	widget = function()
		local items = Operators.trading:get_sell_items()
		local widgets = {}
		for index = 1,5 do
			table.insert(widgets, Widgets.Uitradeslot(items[index], index, false))
		end
		return widgets
	end}

Ui:add_widget{
	state = "trading",
	widget = function()
		if Operators.trading:is_acceptable() then
			return Widgets.Uibutton("Accept", function() Operators.trading:accept() end)
		else
			return Widgets.Uilabel("You need to pay more.")
		end
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "trading/buy",
	init = function()
		local items = Operators.trading:get_shop_items()
		local widgets = {}
		for index,item in ipairs(items) do
			table.insert(widgets, Widgets.Uitradeitem(item, index, true))
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
				icon = item.spec.icon,
				slot = slot}
			table.insert(widgets, Widgets.Uitradeitem(data, index, false))
		end
		return widgets
	end}
