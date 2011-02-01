Container = {}
Container.dialogs = {}
Container.lists = {}

function Container.clicked(self, id, slot)
	if Target.active then
		Target:select_container(id, slot)
	else
		Drag:clicked_container(id, slot)
	end
end

--- Creates a new container.
-- @param self Container class.
-- @param id Unique container number.
-- @param size Number of rows.
-- @param own True if the inventory of the player.
function Container.create(self, id, size, own)
	local list = Widgets.ItemList{size = size, pressed = function(w, r) self:clicked(id, r) end}
	self.lists[id] = list
	if own then
		Inventory:setup(id, list)
	else
		-- TODO: Closing the dialog should notify the server.
		local dialog = Widgets.Popup{cols = 1, rows = 2}
		local button = Widgets.Button{text = "Close", pressed = function() dialog.visible = false end}
		dialog.items = list
		dialog:set_request{width = 200}
		dialog:set_expand{col = 1, row = 1}
		dialog:set_child{col = 1, row = 1, widget = list}
		dialog:set_child{col = 1, row = 2, widget = button}
		self.dialogs[id] = dialog
		dialog.floating = true
	end
end

--- Closes a container.
-- @param self Container class.
-- @param id Unique container number.
function Container.close(self, id)
	local dialog = self.dialogs[id]
	if Inventory.id == id then
		Inventory:setup(nil, nil)
		Inventory:hide()
	end
	if dialog then
		dialog.floating = false
	end
	self.dialogs[id] = nil
	self.lists[id] = nil
end

Container.get_item = function(clss, args)
	local list = clss.lists[args.inv]
	if not list then return end
	local obj = list:get_item{slot = args.slot}
	if not obj then return end
	if not obj.text or #obj.text == 0 then return end
	return obj
end

--- Inserts an item.
-- @param self Container class.
-- @param id Unique container number.
-- @param slot Slot number.
-- @param model Model code.
-- @param name Object name.
-- @param count Object count.
function Container.insert_item(self, id, slot, name, count)
	local list = self.lists[id]
	if list then
		local spec = Itemspec:find{name = name}
		local icon = spec and spec.icon
		list:set_item{slot = slot, icon = icon, name = name, count = count}
	end
end

--- Removes an item.
-- @param self Container class.
-- @param id Unique container number.
-- @param slot Slot number.
function Container.remove_item(self, id, slot)
	local list = self.lists[id]
	if list then
		list:set_item{slot = slot}
	end
end
