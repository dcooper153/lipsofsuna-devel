Ui:add_state{
	state = "loot",
	label = "Loot",
	init = function()
		local object = Object:find{id = Client.data.inventory.id}
		if not object then return end
		local widgets = {}
		for index = 1,object.inventory.size do
			local item = object.inventory:get_object_by_index(index)
			local slot = object.inventory:get_slot_by_index(index)
			local data = item and {
				text = item.spec.name,
				count = item.count or 1,
				icon = item.spec.icon}
			table.insert(widgets, Widgets.Uiinvloot(object.id, data, index, slot))
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
