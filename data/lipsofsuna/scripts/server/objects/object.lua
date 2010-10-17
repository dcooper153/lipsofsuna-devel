local oldanimate = Object.animate

Object.contact_damage = 0.03

Object.animate = function(self, args)
	if oldanimate(self, args) then
		args.type = "object-animated"
		args.object = self
		Vision:event(args)
		return true
	end
end

Object.blade = function(self, name)
	return Vector(0, 0, 0)
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Object.clone = function(self)
	local data = string.gsub(self:save(), "id=[0-9]*,", "")
	return Object:load(data)
end

Object.effect = function(self, args)
	if args.effect then
		Vision:event{type = "object-effect", object = self, effect = args.effect}
	end
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
Object.get_free_id = function(clss)
	while true do
		local id = math.random(0x0000001, 0x0FFFFFF)
		if not Object:find{id = id} then
			local rows = Serialize.db:query("SELECT id FROM objects WHERE id=?;", {id})
			if not rows[1] then
				return id
			end
		end
	end
end

--- Gets a full name string for the object.
-- @param self Object.
-- @return String.
Object.get_name_with_count = function(self)
	local name = self.name or "unnamed object"
	local count = self:get_count()
	if count > 1 then
		name = "" .. count .. "x " .. name
	end
	return name
end

--- Finds a targeted object.
-- @param self Object.
-- @param where Inventory number or zero for world.
-- @param what Inventory slot number or object number for world.
-- @return Object or nil.
Object.find_target = function(self, where, what)
	if where == 0 then
		return Object:find{id = what, point = self.position, radius = 5}
	else
		local inventory = Inventory:find{id = where}
		if inventory and inventory:subscribed{object = self} then
			return inventory:get_object{slot = what}
		end
	end
	return nil
end

--- Causes the object to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Object.damaged = function(self, amount)
end

--- Gets the number of copied stacked to the object.
-- @param self Object.
-- @return Integer.
Object.get_count = function(self)
	return self.count or 1
end

--- Creates a new object from a data string.
-- @param clss Object class.
-- @param data Data string.
-- @return Object.
Object.load = function(clss, data)
	local func = assert(loadstring("return function()\n" .. data .. "\nend"))()
	if func then return func() end
end

--- Subtracts stacked objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count to subtract.</li></ul>
Object.subtract = function(self, args)
	local c = args and args.count or 1
	if self:get_count() > c then
		self:set_count{count = self:get_count() - c}
	else
		self:detach()
		self:purge()
	end
end

--- Merges or adds an item to the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to add.</li></ul>
-- @return True if succeeded.
Object.add_item = function(self, args)
	if not args.object then return end
	local inv = Inventory:find{owner = self}
	if not inv then return end
	if not inv:merge_object{object = args.object} then return end
	return true
end

--- Finds an item from the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li></ul>
-- @return Object or nil.
Object.find_item = function(self, args)
	local inv = Inventory:find{owner = args.user}
	if not inv then return end
	return inv:find_object{name = args.name}
end

--- Splits items from the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li>
--   <li>name: Item name.</li></ul>
-- @return Object or nil.
Object.split_items = function(self, args)
	local inv = Inventory:find{owner = self}
	if not inv then return end
	local obj = inv:find_object{name = args.name}
	if not obj then return end
	obj = obj:split(args)
	obj:detach()
	return obj
end

--- Subtracts items from the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count to subtract.</li>
--   <li>name: Name to match.</li>
--   <li>type: Type to match.</li></ul>
-- @return True if succeeded.
Object.subtract_items = function(self, args)
	local inv = Inventory:find{owner = self}
	if not inv then return end
	local obj = inv:find_object(args)
	if not obj then return end
	obj:subtract(args)
	return true
end

--- Sets member variables from an array.
-- @param self Object.
-- @param args Arguments.
Object.set = function(self, args)
	if args then
		local realized = nil
		for k,v in pairs(args) do
			if k ~= "realized" then
				self[k] = v
			else
				realized = v
			end
		end
		if realized ~= nil then
			self.realized = realized
		end
	end
end

--- Sets the number of copies stacked in the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count.</li>
Object.set_count = function(self, args)
	self.count = args.count;
	-- Update inventory.
	local inv = Inventory:find{object = self}
	if inv then
		for k,v in pairs(inv.slots) do
			if v == self then
				inv:update_slot{slot = k}
				break
			end
		end
	end
	-- Update slots.
	local slots = Slots:find{object = self}
	if slots then
		for k,v in pairs(slots.slots) do
			if v.object == self then
				slots:update_slot{slot = k}
				break
			end
		end
	end
end

--- Detaches the object from the world or any container.
-- @param self Object.
Object.detach = function(self)
	-- Detach from world.
	self.realized = false
	-- Detach from inventory.
	local inv = Inventory:find{object = self}
	if inv then
		for k,v in pairs(inv.slots) do
			if v == self then
				inv:set_object{slot = k}
				break
			end
		end
	end
	-- Detach from slots.
	local slots = Slots:find{object = self}
	if slots then
		for k,v in pairs(slots.slots) do
			if v.object == self then
				slots:set_object{slot = k}
				break
			end
		end
	end
end

--- Called when the object is being equipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
-- @param True on success.
Object.equipped = function(self, user, slot)
	return false
end

--- Called when the object is being unequipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
-- @param True on success.
Object.unequipped = function(self, user, slot)
	return true
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Object.die = function(self)
	self.realized = false
	self:purge()
end

--- Merges the objects if they're similar.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to merge to this one.</li></ul>
-- @return True if merged successfully.
Object.merge = function(self, args)
	local obj = args.object
	if self.itemspec and self.itemspec == args.object.itemspec and self.itemspec.stacking then
		self.count = self:get_count() + obj:get_count()
		obj.realized = false
		return true
	end
end

Object.purge = function(self)
	Serialize.db:query("DELETE FROM objects WHERE id=?;", {self.id})
end

--- Sends a packet to the client controlling the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>packet: Packet. (required)</li>
--   <li>reliable: False for unreliable.</li></ul>
Object.send = function(self, args)
	if self.client then
		Network:send{client = self.client, packet = args.packet, reliable = args.reliable}
	end
end

--- Sends a chat message to all players near the object.
-- @param self Speaking object.
-- @param msg Message to send.
Object.say = function(self, msg)
	if msg then
		Vision:event{type = "object-speech", object = self, message = msg}
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Object.write = function(self)
	return "local self=Object{" ..
		"angular=" .. serialize_value(self.angular) .. "," ..
		"id=" .. serialize_value(self.id) .. "," ..
		"mass=" .. serialize_value(self.mass) .. "," ..
		"name=" .. serialize_value(self.name) .. "," ..
		"model=" .. serialize_value(self.model_name) .. "," ..
		"position=" .. serialize_value(self.position) .. "," ..
		"rotation=" .. serialize_value(self.rotation) .. "}\n" ..
		"return self"
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Object.use_cb = function(self, user)
	Dialog:start{object = self, user = user}
end
