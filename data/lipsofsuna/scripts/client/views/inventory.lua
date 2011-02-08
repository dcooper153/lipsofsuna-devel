Views.Inventory = Class(Widget)
Views.Inventory.mode = "inventory"

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 3, rows = 3, spacings = {0, 0}})
	self.title = Widgets.Frame{style = "title", text = "Inventory"}
	self.containers = Widget{rows = 1}
	self.containers:set_expand{row = 1}
	self.equipment = Widgets.Equipment{pressed = function(widget, slot)
		if Target.active then
			Target:select_equipment(slot)
		else
			Drag:clicked_equipment(nil, slot)
		end
	end}
	self:set_request{width = 100}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.equipment}
	self:set_child{col = 2, row = 3, widget = self.containers}
	self:set_expand{col = 3, row = 2}
	return self
end

--- Adds a container to the inventory.
-- @param self Inventory view.
-- @param widget Container widget.
-- @param own True if this is the main inventory.
Views.Inventory.add_container = function(self, widget, own)
	if own then
		widget.button_close.visible = false
		self.container = widget
		self:set_child{col = 1, row = 3, widget = widget}
	else
		widget.button_close.visible = true
		self.containers:append_col(widget)
		widget.closed = function(w) self:remove_container(w) end
		Gui:set_mode("inventory")
	end
end

--- Removes a container from the inventory view.
-- @param self Inventory view.
-- @param widget Container widget.
Views.Inventory.remove_container = function(self, widget)
	-- Remove main inventory.
	if self.container == widget then
		self.container = nil
		self:set_child{col = 1, row = 3}
	end
	-- Remove looted containers.
	for i = 1,self.containers.cols do
		local w = self.containers:get_child{col = i, row = 1}
		if w == widget then
			self.containers:remove{col = i}
			break
		end
	end
end

Views.Inventory.back = function(self)
	Gui:set_mode("menu")
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
	-- Close containers.
	for i = 1,self.containers.cols do
		local w = self.containers:get_child{col = i, row = 1}
		w:close()
	end
end

--- Enters the inventory view.
-- @param self Inventory view.
Views.Inventory.enter = function(self)
end

--- Gets the contents of an inventory or an equipment slot.
-- @param self Inventory view.
-- @param args Arguments.<ul>
--   <li>id: Container ID.</li></ul>
--   <li>slot: Slot name.</li></ul>
-- @return Item widget or nil.
Views.Inventory.get_item = function(self, args)
	if type(args.slot) == "string" then
		-- Equipment slot.
		local obj = self.equipment.dict_name[args.slot]
		if not obj then return end
		if not obj.text or #obj.text == 0 then return end
		return obj
	else
		-- Inventory slot.
		local cont = Widgets.Container:find(args.id or self.id)
		if not cont then return end
		local obj = cont.item_list:get_item{slot = args.slot}
		if not obj then return end
		if not obj.text or #obj.text == 0 then return end
		return obj
	end
end

--- Sets the contents of an equipment slot.
-- @param self Inventory view.
-- @param slot Slot name.
-- @param name Item name.
-- @param count Item count.
Views.Inventory.set_item = function(self, slot, name, count)
	local widget = self.equipment.dict_name[slot]
	if widget then
		local spec = Itemspec:find{name = name}
		widget.text = name or ""
		widget.icon = spec and spec.icon
		widget.count = count or 1
	end
end

------------------------------------------------------------------------------

Views.Inventory.inst = Views.Inventory()

-- Updates items of the equipment display.
Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,i,count,name,slot = event.packet:read("uint32", "uint32", "string", "string")
	if ok and Player.object and Player.object.id == i then
		Views.Inventory.inst:set_item(slot, name, count)
	end
end}
