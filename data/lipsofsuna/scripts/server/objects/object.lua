local oldanimate = Object.animate
local oldgetter = Object.getter
local oldsetter = Object.setter
local objspec = Spec{name = "object", type = "object"}

Object.contact_damage = 0.03

Object.getter = function(self, key)
	if key == "spec" then
		return oldgetter(self, key, value) or objspec
	else
		return oldgetter(self, key)
	end
end

Object.setter = function(self, key, value)
	if key == "contact_cb" then
		oldsetter(self, key, value)
		self.contact_events = (type(value) == "function")
	else
		oldsetter(self, key, value)
	end
end

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

--- Fires or throws the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile. (required)</li>
--   <li>point: Firing point relative to the owner.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
-- @return True on success.
Object.fire = function(self, args)
	if not args.owner or not args.feat then return end
	-- Enable collision callback.
	if args.collision then
		self.contact_cb = function(_, result)
			if result.object == self.owner then return end
			args.feat:apply{
				attacker = args.owner,
				point = result.point,
				projectile = self,
				target = result.object,
				tile = result.tile}
			self.realized = false
			self.contact_cb = nil
		end
	end
	-- Enable destruction timer.
	if args.timer then
		Timer{delay = args.timer, func = function(timer)
			self:die()
			timer:disable()
		end}
	end
	-- Add the projectile to the world.
	local src,dst = args.owner:get_attack_ray()
	local vec = args.owner.rotation * (dst - src)
	self:detach()
	self.owner = args.owner
	self.position = args.owner.position + args.owner.rotation * src
	if args.owner.tilt then
		self.rotation = args.owner.rotation * args.owner.tilt
	else
		self.rotation = args.owner.rotation
	end
	self.velocity = vec:normalize() * (args.speed or 20)
	self.save = false
	self.realized = true
end

--- Checks if the object is stuck and tries to fix it if it is.
-- @param self Object.
-- @return True if not permanently stuck.
Object.stuck_check = function(self)
	if self:get_stuck() then
		self.stuck = (self.stuck or 0) + 2
		if self.stuck < 10 then
			self:stuck_fix()
		else
			print("Warning: An object was deleted because it was permanently stuck!")
			self.realized = false
			return
		end
	elseif self.stuck then
		if self.stuck > 1 then
			self.stuck = self.stuck -1
		else
			self.stuck = nil
		end
	end
	return true
end

--- Fixes the position of a stuck object.
-- @param self Object.
-- @return True if fixing succeeded.
Object.stuck_fix = function(self)
	-- Get the tile position of the object.
	local src = self:get_tile_range()
	-- Find the closest empty tile.
	-- FIXME: The object doesn't necessarily fit inside one tile.
	local t,p = Voxel:find_tile{match = "empty", point = src * Config.tilewidth, radius = 5 * Config.tilewidth}
	if not t then return end
	-- Move the object to the empty tile.
	self.position = (p + Vector(0.5, 0.1, 0.5)) * Config.tilewidth
	return true
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

--- Returns the range of tiles occupied by the object.
-- @param self Object.
-- @return Start vector, size vector.
Object.get_tile_range = function(self)
	-- TODO: Should depend on species.
	local size = Vector(1,self.spec.type == "creature" and 2 or 1,1)
	local src = self.position * Config.tilescale
	src.x = math.floor(src.x)
	src.y = math.floor(src.y + 0.1)
	src.z = math.floor(src.z)
	return src, src + size - Vector(1,1,1)
end

--- Returns true if the object is stuck inside a wall.
-- @param self Object.
-- @return True if stuck. 
Object.get_stuck = function(self, value)
	if not self.realized then return end
	local src,dst = self:get_tile_range()
	return not Voxel:check_empty(src, dst)
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

--- Merges or adds an item to the slots or inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to add.</li></ul>
-- @return True if succeeded.
Object.add_item = function(self, args)
	if not args.object then return end
	if not self.inventory then return end
	if self.inventory:merge_object{object = args.object} then return true end
end

--- Finds an item from the inventory.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li>
--   <li>slot: Slot name or number.</li></ul>
-- @return Object or nil.
Object.get_item = function(self, args)
	if not self.inventory then return end
	if args.slot then return self.inventory:get_object(args) end
	if args.name then return self.inventory:find_object(args) end
end

--- Sets the item in a specific inventory slot.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to add.</li>
--   <li>slot: Slot name or number.</li></ul>
Object.set_item = function(self, args)
	if not args.object then return end
	if not self.inventory then return end
	self.inventory:set_object(args)
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
	local inventory = Inventory:find{object = self}
	if inventory then
		for k,v in pairs(inventory.slots) do
			if v == self then
				inventory:update_slot{slot = k}
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
	if self.spec == args.object.spec and self.spec.stacking then
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
