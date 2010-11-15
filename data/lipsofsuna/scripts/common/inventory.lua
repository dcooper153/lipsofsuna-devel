--- Inventory class.
-- @name Inventory
-- @class table
Inventory = Class()
Inventory.objectdict = {}
Inventory.ownerdict = {}
setmetatable(Inventory.objectdict, {__mode = "kv"})
setmetatable(Inventory.ownerdict, {__mode = "k"})

--- Finds an inventory by owner or stored object.
-- @param clss Inventory class.
-- @param args Arguments.<ul>
--   <li>id: Inventory ID.</li>
--   <li>object: Stored object.</li>
--   <li>owner: Owner object.</li></ul>
-- @return Inventory or nil.
Inventory.find = function(clss, args)
	if args.id then
		local owner = Object:find{id = args.id}
		return owner and clss.ownerdict[owner]
	elseif args.owner then
		return clss.ownerdict[args.owner]
	elseif args.object then
		return clss.objectdict[args.object]
	end
end

--- Finds an object in the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>name: Item name to match.</li>
--   <li>type: Item category name to match.</li></ul>
-- @return Object and slot index or nil.
Inventory.find_object = function(self, args)
	for k,v in pairs(self.slots) do
		if (not args.name or (v.name and v.name == args.name)) and
		   (not args.type or (v.itemspec and v.itemspec.categories[args.type])) then
			return v, k
		end
	end
end

--- Creates a new inventory.
-- @param clss Inventory class.
-- @param args Arguments.
-- @return New inventory.
Inventory.new = function(clss, args)
	local s = Class.new(clss, args)
	s.size = args.size or 10
	s.slots = {}
	s.listeners = {}
	setmetatable(s.listeners, {__mode = "k"})
	if s.owner then
		if clss.ownerdict[s.owner] then error("object already has inventory") end
		clss.ownerdict[s.owner] = s
	end
	return s
end

--- Gets the first empty slot in the inventory.
-- @param self Inventory.
Inventory.get_empty_slot = function(self)
	for slot = 1,self.size do
		if not self.slots[slot] then return slot end
	end
end

--- Gets an object in a slot.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>slot: Slot index.</li></ul>
-- @return Object or nil.
Inventory.get_object = function(self, args)
	return args.slot and self.slots[args.slot]
end

--- Intelligently adds an object to the inventory.
-- <br/>
-- If no slot number is specified explicitly, loops through all objects in the
-- inventory, trying to merge the object with each of them, returning after the
-- first successful merge. If no merge succeeds, tries to add the object to the
-- first empty slot. If there are no empty slots, returns without adding the
-- object.
-- <br/>
-- If an explicit slot number is requested, checks if the slot is empty. If so,
-- the object is added. Otherwise, a merge is attempted. If the merge fails,
-- the function returns without adding the object.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>exclude: Slot number to exclude.</li>
--   <li>object: Object.</li>
--   <li>slot: Slot number if requiring a specific slot.</li></ul>
-- @return True if merged or added.
Inventory.merge_object = function(self, args)
	if args.slot then
		-- Merge or insert to specific slot.
		local object = self:get_object{slot = args.slot}
		if not object then
			args.object:detach()
			self:set_object(args)
			return true
		else
			if object:merge{object = args.object} then
				args.object:detach()
				self:update_slot{slot = args.slot}
				return true
			end
			return false
		end
	else
		-- Merge or insert to any slot.
		for slot = 1,self.size do
			if slot ~= args.exclude then
				local object = self:get_object{slot = slot}
				if object and object:merge{object = args.object} then
					args.object:detach()
					self:update_slot{slot = slot}
					return true
				end
			end
		end
		local slot = self:get_empty_slot()
		if not slot then return end
		args.object:detach()
		self:set_object{slot = slot, object = args.object}
		return true
	end
end

--- Subscribes an object to the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>callback: Callback.</li>
--   <li>object: Object to subscribe.</li></ul>
Inventory.subscribe = function(self, args)
	if args.object and args.callback then
		self.listeners[args.object] = args.callback
		args.callback{type = "inventory-subscribed", inventory = self}
		for k,v in pairs(self.slots) do
			args.callback{type = "inventory-changed", inventory = self, object = v, slot = k}
		end
	end
end

--- Checks if an object is subscribed to the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>object: Object.</li></ul>
-- @return True if subscribed.
Inventory.subscribed = function(self, args)
	return self.listeners[args.object] ~= nil
end

--- Unsubscribes an object from the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>object: Object to unsubscribe.</li></ul>
Inventory.unsubscribe = function(self, args)
	local v = self.listeners[args.object]
	if v then
		v.callback{type = "inventory-unsubscribed", inventory = self}
		self.listeners[args.object] = nil
	end
end

--- Sets the object of a slot.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>slot: Slot index.</li>
--   <li>object: Object.</li></ul>
Inventory.set_object = function(self, args)
	if not args.slot then return end
	local v = self.slots[args.slot]
	-- Maintain parent list.
	if args.object and Inventory.objectdict[args.object] then error("object already in inventory") end
	if v then Inventory.objectdict[v] = nil end
	if args.object then Inventory.objectdict[args.object] = self end
	-- Add to slot.
	if args.object then args.object:detach() end
	self.slots[args.slot] = args.object
	self:update_slot(args)
end

Inventory.update_slot = function(self, args)
	local o = self:get_object(args)
	for k,v in pairs(self.listeners) do
		v{type = "inventory-changed", inventory = self, object = o, slot = args.slot}
	end
end
