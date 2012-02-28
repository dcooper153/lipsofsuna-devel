Views.Inventory = Class(Widget)

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 3, rows = 1, spacings = {0, 0}})
	return self
end

--- Adds a container to the inventory.
-- @param self Inventory view.
-- @param widget Container widget.
-- @param type Container type number.
-- @param own True if this is the main inventory.
Views.Inventory.add_container = function(self, widget, own)
	if own then
		self.container = widget
	end
end

--- Removes a container from the inventory view.
-- @param self Inventory view.
-- @param widget Container widget.
Views.Inventory.remove_container = function(self, widget)
	if self.container == widget then
		self.container = nil
	end
end

Views.Inventory.back = function(self)
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
	-- Close containers.
	for i = self.containers.cols,1,-1 do
		local w = self.containers:get_child(i, 3)
		w:close()
	end
	-- Close trading.
	if self.trading.visible then
		self:set_trading()
		self.trading:close()
	end
end

--- Enters the inventory view.
-- @param self Inventory view.
-- @param from Name of the previous mode.
Views.Inventory.enter = function(self, from, level)
end

--- Gets the contents of an inventory or an equipment slot.
-- @param self Inventory view.
-- @param args Arguments.<ul>
--   <li>id: Container ID.</li></ul>
--   <li>slot: Slot name.</li></ul>
-- @return Item widget or nil.
Views.Inventory.get_item = function(self, args)
	local cont = Widgets.Container:find(args.id or self.id)
	if not cont then return end
	return cont:get_item(args)
end

--- Sets the contents of an equipment slot.
-- @param self Inventory view.
-- @param slot Slot name.
-- @param name Item name.
-- @param count Item count.
Views.Inventory.set_item = function(self, slot, name, count)
	print("FIXME: Views.Inventory.set_item")
--[[
	local widget = self.equipment.dict_name[slot]
	if widget then
		local spec = Itemspec:find{name = name}
		widget.text = name or ""
		widget.icon = spec and spec.icon
		widget.count = count or 1
	end]]
end

--- Enables or disables trading.
-- @param self Inventory view.
-- @param value True to enable.
Views.Inventory.set_trading = function(self, value)
end

--- Sets the displayed weight and burdening limit.
-- @param self Inventory view.
-- @param weight Weight in kilograms.
-- @param limit Burdening limit in kilograms.
Views.Inventory.set_weight = function(self, weight, limit)
end
