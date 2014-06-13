local UiTransition = require("ui/widgets/transition")

Ui:add_state{
	state = "loot",
	label = "Loot",
	init = function()
		local object = Main.objects:find_by_id(Client.data.inventory.id)
		if not object then return end
		local widgets = {}
		for index = 1,object.spec.inventory_size do
			local item = object.inventory:get_object_by_index(index)
			local slot = object.inventory:get_slot_by_index(index)
			local data = item and {
				text = item.spec.name,
				count = item:get_count(),
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uiinvloot(object:get_id(), data, index, slot))
		end
		return widgets
	end}

Ui:add_widget{
	state = "loot",
	widget = function()
		return UiTransition("Store", "store")
	end}
