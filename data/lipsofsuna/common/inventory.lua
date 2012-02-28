--- Combined inventory and equipment item storage.<br/>
-- A generic inventory class that allows one to easily store and retrieve
-- items. Both objects and strings are supported as items so the class is
-- usable to both the client and the server.
-- @name Inventory
-- @class table
Inventory = Class()
Inventory.class_name = "Inventory"

--- Creates a new inventory.
-- @param clss Inventory class.
-- @param args Arguments.
-- @return New inventory.
Inventory.new = function(clss, args)
	local self = Class.new(clss, args)
	self.size = args and args.size or 10
	self.stored = {}
	self.equipped = {}
	self.listeners = setmetatable({}, {__mode = "k"})
	return self
end

--- Calculates the weight of the contained items.
-- @param self Inventory.
-- @return Weight in kilograms.
Inventory.calculate_weight = function(self)
	local w = 0
	for k,v in pairs(self.stored) do
		w = w + v.spec.mass_inventory * v.count + v.inventory:calculate_weight()
	end
	return w
end

--- Removes all objects from the inventory.
-- @param self Inventory.
Inventory.clear = function(self)
	for k,v in pairs(self.stored) do
		if v then self:set_object(k) end
	end
	self.stored = {}
	self.equipped = {}
end

--- Counts objects that maches the name and type.
-- @param self Inventory.
-- @param name Item name to match.
-- @return Object count.
Inventory.count_objects_by_name = function(self, name)
	local count = 0
	for k,v in pairs(self.stored) do
		if v.spec.name == name then
			count = count + v.count
		end
	end
	return count
end

--- Finds an object in the inventory by type.
-- @param self Inventory.
-- @param type Item category name to match.
-- @return Object and inventory index, or nil.
Inventory.count_objects_by_type = function(self, type)
	local count = 0
	for k,v in pairs(self.stored) do
		if v.spec.categories[type] then
			count = count + v.count
		end
	end
	return count
end

--- Automatically equips the best set of objects.
-- @param self Inventory.
-- @param args Arguments.
Inventory.equip_best_objects = function(self)
	-- Get the owner object.
	local owner = Object:find{id = self.id}
	if not owner then return end
	-- Loop through all available equipment slots.
	for name in pairs(owner.spec.equipment_slots) do
		-- Find the best item to place to the slot.
		local best = nil
		local best_score = -1
		for index,item in pairs(self.stored) do
			if item.spec.equipment_slot == name then
				local score = item:get_equip_value(owner)
				if not best or score < best_score then
					best = index
					best_score = score
				end
			end
		end
		-- Place the best item to the slot.
		if best then self:equip_index(best, name) end
	end
end

--- Equips an inventory item.
-- @param self Inventory.
-- @param index Inventory index.
-- @param slot Equipment slot name.
Inventory.equip_index = function(self, index, slot)
	local o = self.stored[index]
	if not o then return end
	-- Make sure that the item isn't equipped in another slot.
	self:unequip_index(index)
	-- Unequip items in slots reserved by the item.
	self:unequip_slot(slot)
	if o.spec.equipment_slots_reserved then
		for k in pairs(o.spec.equipment_slots_reserved) do
			self:unequip_slot(k)
		end
	end
	-- Equip the item.
	self.equipped[slot] = index
	-- Notify listeners.
	for k,v in pairs(self.listeners) do
		v{type = "inventory-equipped", index = index, inventory = self, object = o, slot = slot}
	end
	-- Notify vision.
	if Vision then
		local parent = Object:find{id = self.id}
		Vision:event{type = "object-equip", id = self.id, index = index, item = o, object = parent, slot = slot}
	end
end

--- Gets the first empty inventory index.
-- @param self Inventory.
Inventory.get_empty_index = function(self)
	for index = 1,self.size do
		if not self.stored[index] then return index end
	end
end

--- Gets the inventory index of the given object.
-- @param self Inventory.
-- @param object Object.
-- @return Inventory index, or nil.
Inventory.get_index_by_object = function(self, object)
	for k,v in pairs(self.stored) do
		if v == object then return k end
	end
end

--- Gets the inventory index of the object in the given equipment slot.
-- @param self Inventory.
-- @param slot Equipment slot name.
-- @return Inventory index, or nil.
Inventory.get_index_by_slot = function(self, slot)
	return self.equipped[slot]
end

--- Returns the object in the given inventory index.
-- @param self Inventory.
-- @param index Inventory index.
-- @return Object, or nil.
Inventory.get_object_by_index = function(self, index)
	return self.stored[index]
end

--- Finds an object in the inventory by name.
-- @param self Inventory.
-- @param name Item name to match.
-- @return Object and inventory index, or nil.
Inventory.get_object_by_name = function(self, name)
	for k,v in pairs(self.stored) do
		if v.name == name then
			return v, k
		end
	end
end

--- Finds an object in the inventory by type.
-- @param self Inventory.
-- @param type Item category name to match.
-- @return Object and inventory index, or nil.
Inventory.get_object_by_type = function(self, type)
	for k,v in pairs(self.stored) do
		if v.spec.categories[type] then
			return v, k
		end
	end
end

--- Returns the object in the given equipment slot.
-- @param self Inventory.
-- @param slot Equipment slot name.
-- @return Object, or nil.
Inventory.get_object_by_slot = function(self, slot)
	local i = self.equipped[slot]
	if not i then return end
	return self.stored[i]
end

--- Gets the equipment slot of the object in the given inventory slot.
-- @param self Inventory.
-- @param index Inventory index.
-- @return Equipment slot name, or nil.
Inventory.get_slot_by_index = function(self, index)
	for k,v in pairs(self.equipped) do
		if v == index then return k end
	end
end

--- Gets the equipment slot of the given object.
-- @param self Inventory.
-- @param index Inventory index.
-- @return Equipment slot name, or nil.
Inventory.get_slot_by_object = function(self, object)
	for k,v in pairs(self.equipped) do
		if self.stored[v] == object then return k end
	end
end

--- Checks if the object is subscribed to the inventory.
-- @param self Inventory.
-- @param object Object.
-- @return True if subscribed.
Inventory.is_subscribed = function(self, object)
	return self.listeners[object] ~= nil
end

--- Intelligently adds an object to the inventory.<br/>
-- Loops through all objects in the inventory, trying to merge the object with
-- each of them. If no merge succeeds, the object is added to the first free
-- inventory index.
-- @param self Inventory.
-- @param object Object.
-- @param exclude Slot number to exclude, or nil.
-- @return True if merged or added.
Inventory.merge_object = function(self, object, exclude)
	-- Merge with any possible object.
	for index = 1,self.size do
		if index ~= exclude then
			local object1 = self:get_object_by_index(index)
			if object1 and object1:merge(object) then
				object:detach()
				self:update_index(index)
				return true
			end
		end
	end
	-- Insert to any free index.
	local index = self:get_empty_index()
	if not index then return end
	object:detach()
	self:set_object(index, object)
	return true
end

--- Merges the object to the object in the given index.<br/>
-- If the slot is emprt, the object is added. Otherwise, a merge is attempted.
-- If the merge fails, the function returns without adding the object.<br/>
-- @param self Inventory.
-- @param index Inventory index.
-- @param object Object.
-- @return True if merged or added.
Inventory.merge_object_to_index = function(self, index, object)
	-- Merge or insert to a specific inventory index.
	local object1 = self:get_object_by_index(index)
	if not object1 then
		object:detach()
		self:set_object(index, object)
		return true
	elseif object1:merge(object) then
		object:detach()
		self:update_index(index)
		return true
	end
end

--- Merges the object to the inventory or drops it on the floor near the owner object.
-- @param self Inventory.
-- @param object Object to give.
-- @return True if added to inventory, nil if dropped.
Inventory.merge_or_drop_object = function(self, object)
	-- Try to merge.
	if self:merge_object(object) then return true end
	-- Find the owner.
	if not Utils then return end
	local o = Object:find{id = self.id}
	if not o then return end
	-- Drop near the owner.
	local p = Utils:find_drop_point{point = o.position}
	object.position = p or o.position
	object.realized = true
end

--- Removes an object from the inventory.
-- @param self Inventory.
-- @param object Object.
Inventory.remove_object = function(self, object)
	for k,v in pairs(self.stored) do
		if v == object then
			self:set_object(k)
			break
		end
	end
end

--- Sets the object in the given inventory index.
-- @param self Inventory.
-- @param slot Slot name or number.
-- @param object Object, or nil.
Inventory.set_object = function(self, index, object)
	-- Check for changes.
	local oldobj = self.stored[index]
	if oldobj == object then return end
	-- Unequip and remove the old object.
	if oldobj then
		self:unequip_index(index)
		oldobj.parent = nil
	end
	-- Detach the new object from the world.
	if object then
		object:detach()
	end
	-- Add to slot.
	if object then
		object.parent = self.id
	end
	self.stored[index] = object
	self:update_index(index)
end

--- Splits items from the inventory of the object.
-- @param self Object.
-- @param index Inventory index.
-- @param count Split count.
-- @return Object, or nil.
Inventory.split_object_by_index = function(self, index, count)
	local obj = self:get_object_by_index(index)
	if not obj then return end
	obj = obj:split(count)
	obj:detach()
	return obj
end

--- Splits items from the inventory of the object.
-- @param self Object.
-- @param name Object name.
-- @param count Split count.
-- @return Object, or nil.
Inventory.split_object_by_name = function(self, name, count)
	-- TODO: Splitting from multiple piles.
	local obj = self:get_object_by_name(name)
	if not obj then return end
	obj = obj:split(count)
	obj:detach()
	return obj
end

--- Subscribes the object to the inventory.
-- @param self Inventory.
-- @param object Object.
-- @param callback Callback.
Inventory.subscribe = function(self, object, callback)
	if not self.listeners[object] then
		self.listeners[object] = callback
		callback{type = "inventory-subscribed", inventory = self}
		for k,v in pairs(self.stored) do
			callback{type = "inventory-changed", inventory = self, object = v, index = k}
		end
		for k,v in pairs(self.equipped) do
			local o = self.stored[v]
			callback{type = "inventory-equipped", index = v, inventory = self, object = o, slot = k}
		end
	end
end

--- Subtracts objects from the inventory by object type.
-- @param self Inventory.
-- @param name Object name to match.
-- @param count Count to subtract.
-- @return True if succeeded.
Inventory.subtract_objects_by_name = function(self, name, count)
	local left = count
	for k,v in pairs(self.stored) do
		if v.name == name then
			if v.count < left then
				left = left - v.count
				self:set_object(k)
			elseif v.count == left then
				self:set_object(k)
				left = 0
				break
			else
				v:subtract(left)
				left = 0
				break
			end
		end
	end
	return left
end

--- Subtracts objects from the inventory by object name.
-- @param self Inventory.
-- @param type Object type to match.
-- @param count Count to subtract.
-- @return Number of objects that could not be subtracted.
Inventory.subtract_objects_by_type = function(self, type, count)
	local left = count
	for k,v in pairs(self.stored) do
		if v.categories[type] then
			if v.count < left then
				left = left - v.count
				self:set_object(k)
			elseif v.count == left then
				self:set_object(k)
				left = 0
				break
			else
				v:subtract(left)
				left = 0
				break
			end
		end
	end
	return left
end

--- Unequips an inventory item by inventory index.
-- @param self Inventory.
-- @param index Inventory index.
Inventory.unequip_index = function(self, index)
	local o = self.stored[index]
	if not o then return end
	local slot = self:get_slot_by_index(index)
	if not slot then return end
	self.equipped[slot] = nil
	-- Notify listeners.
	for k,v in pairs(self.listeners) do
		v{type = "inventory-unequipped", index = index, inventory = self, object = o, slot = slot}
	end
	-- Notify vision.
	if Vision then
		local parent = Object:find{id = self.id}
		Vision:event{type = "object-unequip", id = self.id, index = index, item = o, object = parent, slot = slot}
	end
end

--- Unequips an inventory item by equipment slot name.
-- @param self Inventory.
-- @param slot Equipment slot name.
Inventory.unequip_slot = function(self, slot)
	local index = self.equipped[slot]
	if not index then return end
	local o = self.stored[index]
	if not o then return end
	self.equipped[slot] = nil
	-- Notify listeners.
	for k,v in pairs(self.listeners) do
		v{type = "inventory-unequipped", index = index, inventory = self, object = o, slot = slot}
	end
	-- Notify vision.
	if Vision then
		local parent = Object:find{id = self.id}
		Vision:event{type = "object-unequip", id = self.id, index = index, item = o, object = parent, slot = slot}
	end
end

--- Unsubscribes an object from the inventory.
-- @param self Inventory.
-- @param object Object.
Inventory.unsubscribe = function(self, object)
	local listener = self.listeners[object]
	if listener then
		listener{type = "inventory-unsubscribed", inventory = self}
		self.listeners[object] = nil
	end
end

--- Emits an update event for the index where the object is.
-- @param self Inventory.
-- @param object Object.
Inventory.update_object = function(self, object)
	local index = self:get_index_by_object(object)
	if not index then return end
	for k,v in pairs(self.listeners) do
		v{type = "inventory-changed", index = index, inventory = self, object = object}
	end
end

--- Emits an update event for the given inventory index.
-- @param self Inventory.
-- @param index Inventory index.
Inventory.update_index = function(self, index)
	local o = self:get_object_by_index(index)
	for k,v in pairs(self.listeners) do
		v{type = "inventory-changed", index = index, inventory = self, object = o}
	end
end
