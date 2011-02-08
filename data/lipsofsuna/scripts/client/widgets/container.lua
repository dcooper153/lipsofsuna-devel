require "client/widgets/frame"

Widgets.Container = Class(Widgets.Frame)
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
--   <li>size: Number of slots in the container.</li></ul>
Widgets.Container.new = function(clss, args)
	local self = Widgets.Frame.new(clss, {cols = 1, rows = 2})
	self.id = args.id
	self.closed = args.closed
	self.button_close = Widgets.Button{text = "Close", pressed = function() self:close() end}
	self.item_list = Widgets.ItemList{size = args.size, pressed = function(w, r) self:pressed(r) end}
	self:set_request{width = 200}
	self:set_expand{col = 1, row = 1}
	self:set_child{col = 1, row = 1, widget = self.item_list}
	self:set_child{col = 1, row = 2, widget = self.button_close}
	clss.dict_id[args.id] = self
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
