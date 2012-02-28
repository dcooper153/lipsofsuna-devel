require "client/widgets/trading"

Widgets.Container = Class()
Widgets.Container.class_name = "Widgets.Container"
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
	local self = Class.new(clss)
	self.id = args.id
	self.size = args.size
	self.spec = args.spec
	self.items = {}
	self.equipment = {}
	clss.dict_id[args.id] = self
	return self
end

--- Called when a slot was pressed.
-- @param self Container widget.
-- @param args Event arguments.
-- @param slot Slot pressed.
Widgets.Container.activated = function(self, args, slot)
	if Target.active then
		Target:select_container(self.id, slot)
	elseif args and args.button == 1 then
		Drag:clicked_container(self.id, slot)
	elseif args and args.button == 3 then
		Commands:use(self.id, slot)
	end
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
		return self.items[args.slot]
	else
		return self.equipment[args.slot]
	end
end

--- Finds an item by name and returns its slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li></ul>
-- @return Slot or nil.
Widgets.Container.find_item = function(self, args)
	-- Search from equipment slots.
	for slot,item in pairs(self.equipment) do
		if item.text == args.name then return slot end
	end
	-- Search from inventory slots.
	for slot,item in pairs(self.items) do
		if item.text == args.name then return slot end
	end
end

--- Adds an extra child widget.
-- @param self Container widget.
-- @param widget Widget.
Widgets.Container.set_extra_widget = function(self, widget)
end

--- Sets the item of a slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>count: Item count.</li>
--   <li>icon: Item icon.</li>
--   <li>name: Item name.</li>
--   <li>slot: Slot name or number.</li></ul>
Widgets.Container.set_item = function(self, index, spec, count)
	if spec then
		self.items[index] = {
			spec = spec,
			text = spec.name,
			count = count or 1,
			icon = spec.icon}
	else
		self.items[index] = nil
	end
end

Widgets.Container.equip_index = function(self, index, slot)
	local item = self.items[index]
	if not item then return end
	item.slot = slot
end

Widgets.Container.unequip_index = function(self, index, slot)
	local item = self.items[index]
	if not item then return end
	item.slot = nil
end

--- Updates the crafting list.
-- @param self Container widget.
Widgets.Container.update = function(self)
end
