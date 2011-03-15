local oldanimate = Object.animate
local oldgetter = Object.getter
local oldsetter = Object.setter
local objspec = Spec{name = "object", type = "object"}

Object.getter = function(self, key)
	if key == "count" then
		return oldgetter(self, key) or 1
	elseif key == "spec" then
		return oldgetter(self, key) or objspec
	else
		return oldgetter(self, key)
	end
end

Object.setter = function(self, key, value)
	if key == "contact_cb" then
		oldsetter(self, key, value)
		self.contact_events = (type(value) == "function")
	elseif key == "count" then
		-- Store the new count.
		if self.count == value then return end
		if value ~= 0 then
			oldsetter(self, key, value)
		else
			oldsetter(self, key, nil)
		end
		-- Update the inventory containing the object.
		local inventory = Inventory:find{object = self}
		if inventory then
			for k,v in pairs(inventory.slots) do
				if v == self then
					inventory:update_slot{slot = k}
					break
				end
			end
		end
	else
		oldsetter(self, key, value)
	end
end

--- Creates a new object from a data string or database entry.
-- @param clss Object class.
-- @param args Arguments.<ul>
--  <li>data: Data string.</li>
--  <li>id: Object ID to search from the database.</li></ul>
-- @return Object or nil.
Object.load = function(clss, args)
	if args.data then
		local func = assert(loadstring("return function()\n" .. args.data .. "\nend"))()
		if func then return func() end
	elseif args.id then
		local rows = Serialize.db:query("SELECT * FROM objects WHERE id=?;", {args.id})
		for k,v in ipairs(rows) do
			local func = assert(loadstring("return function()\n" .. v[3] .. "\nend"))()
			return func and func()
		end
	end
end

--- Default add enemy call.
-- @param self Object.
-- @param object Object to add to the list of enemies.
Object.add_enemy = function(self, object)
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

--- Plays an animation.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>animation: Animation name.</li>
--   <li>permanent: True to keep playing.</li>
--   <li>weight: Blending weight.</li></ul>
-- @return True if started a new animation.
Object.animate = function(self, args)
	-- Maintain channels.
	if args.permanent then
		if not self.animations then self.animations = {} end
		local prev = self.animations[args.channel]
		if prev and prev[1] == args.animation and prev[2] == args.weight then return end
		self.animations[args.channel] = {args.animation, args.weight, Program.time}
	elseif self.animations and args.channel then
		self.animations[args.channel] = nil
	end
	-- Emit a vision event.
	args.type = "object-animated"
	args.object = self
	Vision:event(args)
	return true
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Object.clone = function(self)
	local data = string.gsub(self:save(), "id=[0-9]*,", "")
	return Object:load{data = data}
end

--- Checks if the object contains the given object in its inventory.<br/>
-- The check is recursive so all the containers contained by the object
-- are also tested. The object is also considered to contain itself.
-- @param self Object.
-- @param object Object.
-- @return Container object and slot if found, nil otherwise.
Object.contains_item = function(self, object)
	local p = object
	while p do
		if p == self then return true end
		local inv = Inventory:find{object = p}
		if not inv then return end
		p = inv.owner
	end
end

--- Causes the object to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Object.damaged = function(self, amount)
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

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Object.die = function(self)
	self.realized = false
	self:purge()
end

Object.effect = function(self, args)
	if args.effect then
		Vision:event{type = "object-effect", object = self, effect = args.effect}
	end
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

--- Fires or throws the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile. (required)</li>
--   <li>point: Firing point in world space.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li>
--   <li>weapon: Used weapon.</ul>
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
				tile = result.tile,
				weapon = args.weapon}
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
	self:detach()
	self.owner = args.owner
	self.position = src
	if args.owner.tilt then
		self.rotation = args.owner.rotation * args.owner.tilt
	else
		self.rotation = args.owner.rotation
	end
	self.velocity = (dst - src):normalize() * (args.speed or 20)
	self.save = false
	self.realized = true
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

--- Gets a full name string for the object.
-- @param self Object.
-- @return String.
Object.get_name_with_count = function(self)
	local name = self.name or "unnamed object"
	local count = self.count
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
	src.y = math.floor(src.y + 0.3)
	src.z = math.floor(src.z)
	return src, src + size - Vector(1,1,1)
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
Object.inflict_modifier = function(self, name, strength)
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
Object.loot = function(self, user)
	if self.inventory then
		self.inventory:subscribe{object = user, callback = function(args) user:inventory_cb(args) end}
		self:animate{animation = self.spec.animation_looting, weight = 10}
		self.looted = true
	end
end

--- Merges the objects if they're similar.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to merge to this one.</li></ul>
-- @return True if merged successfully.
Object.merge = function(self, args)
	local obj = args.object
	if self.spec == args.object.spec and self.spec.stacking then
		self.count = self.count + obj.count
		obj.realized = false
		return true
	end
end

Object.purge = function(self)
	Serialize.db:query("DELETE FROM objects WHERE id=?;", {self.id})
end

--- Saves the object to the database.
-- @param self Object.
Object.save = function(self)
	local data = self:write()
	Serialize.db:query("REPLACE INTO objects (id,sector,data) VALUES (?,?,?);", {self.id, self.sector, data})
end

--- Sends a chat message to all players near the object.
-- @param self Speaking object.
-- @param msg Message to send.
Object.say = function(self, msg)
	if msg then
		Vision:event{type = "object-speech", object = self, message = msg}
	end
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
	self.position = (p + Vector(0.5, 0.2, 0.5)) * Config.tilewidth
	return true
end

--- Subtracts stacked objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count to subtract.</li></ul>
Object.subtract = function(self, args)
	local c = args and args.count or 1
	if self.count > c then
		self.count = self.count - c
	else
		self:detach()
		self:purge()
	end
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

--- Teleports the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>marker: Map marker name.</li>
--   <li>position: World position.</li></ul>
-- @return True on success.
Object.teleport = function(self, args)
	-- Set the position.
	if args.marker then
		local marker = Marker:find{name = args.marker}
		if not marker or not marker.position then return end
		self.position = marker.position + Vector(0, 2, 0)
	elseif args.position then
		self.position = args.position
	else return end
	self.realized = true
	return true
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
Object.update_environment = function(self, secs)
	-- Count tiles affecting us.
	if not self.realized then return true end
	local src,dst = self:get_tile_range()
	local res = Voxel:check_range(src, dst)
	-- Stuck handling.
	if res.solid > 0 then
		self.stuck = (self.stuck or 0) + 2
		if self.stuck < 10 then
			self:stuck_fix()
		else
			print("Warning: " .. (self.spec.name or "an object") .. " was deleted because it was permanently stuck!")
			self.realized = false
			return nil, res
		end
	elseif self.stuck then
		if self.stuck > 1 then
			self.stuck = self.stuck -1
		else
			self.stuck = nil
		end
	end
	return true, res
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Object.use_cb = function(self, user)
	if not Dialog:start{object = self, user = user} then
		self:loot(user)
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Object.write = function(self)
	return string.format("local self=Object%s\n%s", serialize{
		angular = self.angular,
		id = self.id,
		mass = self.mass,
		name = self.name,
		model = self.model_name,
		position = self.position,
		rotation = self.rotation},
		"return self")
end
