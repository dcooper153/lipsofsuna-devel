Drag = Class()

--- Called when a container is clicked with the purpose of starting or stopping a drag.
-- @param self Drag.
-- @param inv Inventory number of the container.
-- @param slot Slot number of the countainer.
Drag.clicked_container = function(self, inv, slot)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- slot. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" or self.drag[1] == "inv" then
			Equipment:move(self.drag[1], self.drag[2], self.drag[3], "inv", inv, slot)
			self:clear()
		else
			self:cancel()
		end
		return
	end
	-- Make sure that the dragged item exists.
	local item = Container:get_item{inv = inv, slot = slot}
	if not item then return end
	-- Update the cursor.
	self.drag = {"inv", inv, slot}
	Widgets.Cursor.inst.text = item.text
	Widgets.Cursor.inst.icon = Iconspec:find{name = item.icon}
	-- Hide the dragged item.
	item.drag = true
end

--- Called when a container is clicked with the purpose of starting or stopping a drag.
-- @param self Drag.
-- @param inv Inventory number of the equipment.
-- @param slot Slot number of the countainer.
Drag.clicked_equipment = function(self, inv, slot)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- slot. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" or self.drag[1] == "inv" then
			Equipment:move(self.drag[1], self.drag[2], self.drag[3], "equ", inv, slot)
			self:clear()
		else
			self:cancel()
		end
		return
	end
	-- Make sure that the dragged item exists.
	local item = Equipment:get_item{slot = slot}
	if not item then return end
	-- Update the cursor.
	self.drag = {"equ", inv, slot}
	Widgets.Cursor.inst.text = item.text
	Widgets.Cursor.inst.icon = Iconspec:find{name = item.icon}
	-- Hide the dragged item.
	item.drag = true
end

Drag.cancel = function(self)
	self:clear()
end

Drag.clear = function(self)
	-- Restore items slots back to normal.
	if not self.drag then return end
	if self.drag[1] == "equ" then
		local item = Equipment:get_item{inv = self.drag[2], slot = self.drag[3]}
		if item then item.drag = nil end
	elseif self.drag[1] == "inv" then
		local item = Container:get_item{inv = self.drag[2], slot = self.drag[3]}
		if item then item.drag = nil end
	end
	-- Reset the cursor.
	Widgets.Cursor.inst.text = nil
	Widgets.Cursor.inst.icon = nil
	-- Clear the drag.
	self.drag = nil
end