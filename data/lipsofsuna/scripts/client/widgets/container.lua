Widgets.Container = Class(Widget)
Widgets.Container.dict_id = {}

--- Finds a container widget by ID.
-- @param clss Container widget class.
-- @param id Container ID.
-- @return Container widget or nil.
Widgets.Container.find = function(clss, id)
	return clss.dict_id[id]
end

--- Creates a new container widget.
-- @param clss Container widget class.
-- @param args Arguments.<ul>
--   <li>closed: Close handler.</li>
--   <li>id: Container ID.</li>
--   <li>size: Number of slots in the container.</li>
--   <li>spec: Object spec of the owner.</li></ul>
Widgets.Container.new = function(clss, args)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.id = args.id
	self:set_request{width = 200}
	self:set_expand{col = 1, row = 1}
	clss.dict_id[args.id] = self
	-- Equipment list.
	local slots = nil
	if args.spec and args.spec.equipment_slots then
		for k,v in pairs(args.spec.equipment_slots) do
			slots = true
			break
		end
	end
	if slots then
		self.equipment = Widgets.Equipment{spec = spec, pressed = function(widget, slot)
			if Target.active then
				Target:select_equipment(slot)
			else
				Drag:clicked_equipment(self.id, slot)
			end
		end}
		self:set_child{col = 1, row = 1, widget = self.equipment}
	end
	-- Item list.
	self.item_list = Widgets.ItemList{size = args.size, pressed = function(w, r) self:pressed(r) end}
	self.group = Widgets.Frame{cols = 1, rows = 2}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child{col = 1, row = 1, widget = self.item_list}
	self:set_child{col = 1, row = 2, widget = self.group}
	-- Close button.
	self.closed = args.closed
	self.button_close = Widgets.Button{text = "Close", pressed = function() self:close() end}
	self.group:set_child{col = 1, row = 2, widget = self.button_close}
	return self
end

--- Closes the container.
-- @param self Container widget.
-- @param silent True to not send a network event.
Widgets.Container.close = function(self, silent)
	if not silent then
		Network:send{packet = Packet(packets.INVENTORY_CLOSED, "uint32", self.id)}
	end
	self.dict_id[self.id] = nil
	self:closed()
end

--- Called when the container was closed.
-- @param self Container widget.
Widgets.Container.closed = function(self)
end

--- Gets an item widget in a slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>slot: Slot name or number.</li></ul>
Widgets.Container.get_item = function(self, args)
	if type(args.slot) == "number" then
		local widget = self.item_list.buttons[args.slot]
		if not widget then return end
		if not widget.text or #widget.text == 0 then return end
		return widget
	else
		if not self.equipment then return end
		local widget = self.equipment.dict_name[args.slot]
		if not widget then return end
		if not widget.text or #widget.text == 0 then return end
		return widget
	end
end

--- Called when a slot was pressed.
-- @param self Container widget.
-- @param slot Slot pressed.
Widgets.Container.pressed = function(self, slot)
	if Target.active then
		Target:select_container(self.id, slot)
	else
		Drag:clicked_container(self.id, slot)
	end
end

--- Sets the item of a slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>count: Item count.</li>
--   <li>icon: Item icon.</li>
--   <li>name: Item name.</li>
--   <li>slot: Slot name or number.</li></ul>
Widgets.Container.set_item = function(self, args)
	if type(args.slot) == "number" then
		local widget = self.item_list.buttons[args.slot]
		if not widget then return end
		widget.text = args.name
		widget.count = args.count
		widget.icon = args.icon
	else
		if not self.equipment then return end
		local widget = self.equipment.dict_name[args.slot]
		if not widget then return end
		widget.text = args.name
		widget.count = args.count
		widget.icon = args.icon
	end
end
