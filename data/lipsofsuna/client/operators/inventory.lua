Operators.inventory = Class()
Operators.inventory.data = {}
Operators.inventory.containers = {}

--- Resets the inventory operator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.inventory.reset = function(self)
	self.data = {}
	self.containers = {}
end

--- Adds an inventory subscription.
--
-- Context: The inventory operator must have been initialized.
--
-- @param self Operator.
-- @param id Inventory ID.
Operators.inventory.add_inventory = function(self, id)
	self.containers[id] = true
end

--- Removes an container subscription.
--
-- Context: The inventory operator must have been initialized.
--
-- @param self Operator.
-- @param id Inventory ID.
Operators.inventory.remove_inventory = function(self, id)
	self.containers[id] = nil
end

--- Gets a subscribed inventory by ID.
--
-- Context: The inventory operator must have been initialized.
--
-- @param self Operator.
-- @param id Inventory ID.
-- @returns Inventory, or nil.
Operators.inventory.get_inventory_by_id = function(self, id)
	if not self.containers[id] then return end
	local object = Object:find{id = id}
	if not object then return end
	return object.inventory
end
