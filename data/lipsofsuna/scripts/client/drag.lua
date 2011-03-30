Drag = Class()

--- Cancels the active drag and restores the UI to the original state.
-- @param self Drag.
Drag.cancel = function(self)
	self:clear()
end

--- Clears the drag state after finishing a drag.
-- @param self Drag.
Drag.clear = function(self)
	-- Restore items slots back to normal.
	if not self.drag then return end
	if self.drag[1] == "equ" or self.drag[1] == "inv" then
		local item = Views.Inventory.inst:get_item{id = self.drag[2], slot = self.drag[3]}
		if item then item.drag = nil end
	end
	-- Reset the cursor.
	Widgets.Cursor.inst.widget = nil
	-- Clear the drag.
	self.drag = nil
end

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
			if type(slot) == "number" then
				Equipment:move(self.drag[1], self.drag[2], self.drag[3], "inv", inv, slot)
			else
				Equipment:move(self.drag[1], self.drag[2], self.drag[3], "equ", inv, slot)
			end
			self:clear()
		else
			self:cancel()
		end
		return
	end
	-- Make sure that the dragged item exists.
	local item = Views.Inventory.inst:get_item{id = inv, slot = slot}
	if not item then return end
	-- Update the cursor.
	if type(slot) == "number" then
		self.drag = {"inv", inv, slot, item.text}
	else
		self.drag = {"equ", inv, slot, item.text}
	end
	local spec = Itemspec:find{name = item.text}
	Widgets.Cursor.inst.widget = Widgets.Itemtooltip{count = item.count, spec = spec}
	-- Hide the dragged item.
	item.drag = true
end

--- Called when a quickslot is clicked with the purpose of stopping a drag.
-- @param self Drag.
-- @param index Quickslot index number.
-- @return True if the click was handled by the drag and drop system.
Drag.clicked_quickslot = function(self, index)
	if not self.drag then return end
	if self.drag[1] == "equ" or self.drag[1] == "inv" then
		Quickslots:assign_item(index, self.drag[4])
		self:clear()
	else
		self:cancel()
	end
	return true
end

--- Called when the scene is clicked with the purpose of stopping a drag.
-- @param self Drag.
-- @return True if the click was handled by the drag and drop system.
Drag.clicked_scene = function(self)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- world. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" or self.drag[1] == "inv" then
			Equipment:move(self.drag[1], self.drag[2], self.drag[3], "obj")
			self:clear()
		else
			self:cancel()
		end
		return true
	end
	-- TODO: Dragging from the world to the inventory.
end
