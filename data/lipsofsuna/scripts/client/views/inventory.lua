Views.Inventory = Class(Widget)
Views.Inventory.mode = "inventory"

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.title = Widgets.Frame{style = "title", text = "Inventory"}
	self.inventory = Widgets.Frame{cols = 1, rows = 1}
	self.inventory:set_expand{col = 1, row = 1}
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
	self:set_child{col = 1, row = 3, widget = self.inventory}
	self:set_expand{col = 2, row = 2}
	return self
end

Views.Inventory.back = function(self)
	Gui:set_mode("menu")
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
end

--- Enters the inventory view.
-- @param self Inventory view.
Views.Inventory.enter = function(self)
end

--- Gets the contents of an equipment slot.
-- @param self Inventory view.
-- @param args Arguments.<ul>
--   <li>slot Slot name.</li></ul>
-- @return Item widget or nil.
Views.Inventory.get_item = function(self, args)
	local obj = self.equipment.dict_name[args.slot]
	if not obj then return end
	if not obj.text or #obj.text == 0 then return end
	return obj
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

--- Assigns the player inventory widget to the view.
-- @param self Inventory view.
-- @param widget Player inventory widget.
Views.Inventory.setup = function(self, widget)
	self.itemlist = widget
	self.inventory:set_child{col = 1, row = 1, widget = widget}
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
