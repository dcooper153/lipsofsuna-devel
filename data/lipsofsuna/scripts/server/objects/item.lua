Item = Class(Object)
Item.pickable = true

Item.setter = function(self, key, value)
	if key == "spec" then
		local spec = type(value) == "string" and Itemspec:find{name = value} or value
		if not spec then return end
		Object.setter(self, key, spec)
		self.mass = spec.mass
		self.model = spec.model
		self.name = spec.name
		-- Create the inventory.
		if spec.inventory_size and not self.inventory then
			self.inventory = Inventory{owner = self, size = spec.inventory_size}
			for k,v in pairs(spec.inventory_items) do
				self:add_item{object = Item{spec = Itemspec:find{name = v}}}
			end
		end
		-- Create random loot.
		-- When the map generator or an admin command creates an object, the
		-- random field is set to indicate that random loot should be generated.
		-- The field isn't saved so loot is only created once as expected.
		if self.random and self.inventory and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count then
				num_item = math.random(spec.loot_count[1], spec.loot_count[2])
			else
				num_item = math.random(0, self.inventory.size)
			end
			for i = 1,num_item do
				local cat = spec.loot_categories[math.random(1, num_cat)]
				self:add_item{object = Item{spec = Itemspec:random{category = cat}}}
			end
		end
	else
		Object.setter(self, key, value)
	end
end

--- Creates an item.
-- @param clss Item class.
-- @param args Arguments.<ul>
--   <li>angular: Angular velocity.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the item.</li>
--   <li>rotation: Rotation quaternion of the item.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
-- @return New item.
Item.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("angular")
	copy("count")
	copy("physics", "rigid")
	copy("position")
	copy("random")
	copy("rotation")
	copy("spec")
	copy("looted")
	copy("realized")
	self.update_timer = 0.3 * math.random()
	clss.update_list[self.id] = self
	if self.looted then self:animate("looted") end
	return self
end

--- Creates a copy of the item.
-- @param self Object.
-- @return Object.
Item.clone = function(self)
	return Item{
		spec = self.spec,
		angular = self.angular,
		position = self.position,
		rotation = self.rotation}
end

--- Causes the item to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Item.damaged = function(self, amount)
	-- Items whose spec has no health are indestructible.
	if not self.spec.health then return end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Destroys the item.
-- @param self Object.
Item.die = function(self)
	-- Mark as dead.
	if self.dead then return end
	self.dead = true
	-- Execute destruction actions.
	for k,v in ipairs(self.spec.destroy_actions) do
		if v == "explode" then
			local r1 = 1
			local r2 = (r1 + 3) * Voxel.tile_size
			Particles:create(self.position, "explosion1")
			-- Damage nearby tiles.
			local _,ctr = Voxel:find_tile{point = self.position}
			for x=-r1,r1 do
				for y=-r1,r1 do
					for z=-r1,r1 do
						local o = Vector(x,y,z)
						if o.length < r1 + 0.6 then
							Voxel:damage(nil, ctr + o)
						end
					end
				end
			end
			-- Damage nearby objects.
			for k1,v1 in pairs(Object:find{point = self.position, radius = r2}) do
				local diff = v1.position - self.position
				local frac = 0.3 * diff.length / r2
				local mult = 10 * math.min(100, v1.mass)
				local impulse = diff:normalize() * (mult * (1 - frac))
				v1:impulse{impulse = impulse, point = Vector()}
				v1:damaged(40 * (1 - frac))
			end
		end
	end
	-- Remove from the world.
	Object.die(self)
end

--- Splits items from the stack.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li></ul>
-- @return Object.
Item.split = function(self, args)
	local c = args and args.count or 1
	if c < self.count then
		local o = self:clone()
		self:subtract{count = c}
		o.count = c
		return o
	end
	return self
end

--- Fires or throws the item.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile. (required)</li>
--   <li>point: Firing point relative to the owner.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
-- @return The split and fired item.
Item.fire = function(self, args)
	-- Split a projectile from the stack and fire it.
	if not args.owner or not args.feat then return end
	local proj = self:split()
	Object.fire(proj, args)
	-- Special handling for boomerangs.
	if proj.spec.categories["boomerang"] then
		-- Enable boomerange collisions.
		-- The boomerang mode is disabled when a collision occurs and either the
		-- collision object is damaged or the user catches the boomerang.
		if args.collision then
			proj.contact_cb = function(_, result)
				if result.object == proj.owner then
					-- Owner catch.
					local o = proj.owner:get_item{slot = "hand.R"}
					if not o then
						proj.owner:set_item{slot = "hand.R", object = proj}
					elseif not o:merge{object = proj} then
						proj.owner:add_item{object = proj}
					end
				else
					-- Damage target.
					args.feat:apply{
						attacker = args.user,
						point = result.point,
						projectile = proj,
						target = result.object,
						tile = result.tile}
				end
				-- Disable boomerang mode.
				proj.timer:disable()
				proj.gravity = Config.gravity
				proj:animate("fly stop")
				proj.contact_cb = nil
			end
		end
		-- Enable boomerang rotation.
		if proj.spec.categories["boomerang"] then
			proj.rotated = 0
			proj.rotation = Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi}
			proj:animate("fly start")
			proj.gravity = Vector(0,2,0)
			proj.timer = Timer{delay = 0, func = function(self, secs)
				-- Adjust velocity vector.
				local m = 1.55 * math.pi
				local r = math.min(secs * 1.3 * math.pi, m - proj.rotated)
				proj.velocity = Quaternion{axis = Vector(0,1,0), angle = r} * proj.velocity
				-- Stop after a while.
				proj.rotated = proj.rotated + r
				if proj.rotated >= m then
					self:disable()
					proj.gravity = Config.gravity
				end
			end}
		end
	end
	-- Return the fired projectile.
	return proj
end

--- Called when the object is examined.
-- @param self Object.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send{packet = Packet(packets.MESSAGE, "string", self.spec.name)}
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Item.use_cb = function(self, user)
	-- Actions that take preference over picking up.
	-- Containers are looted instead of being picked up since the player
	-- usually doesn't want to pick up heavy chests. Books are read since
	-- they aren't horribly useful after being read once.
	if self.spec.categories["container"] then
		self:loot(user)
		return
	elseif self.spec.categories["book"] then
		user:send{packet = Packet(packets.BOOK,
			"string", self.spec.name,
			"string", self.spec.book_text)}
		return
	end
	-- Pick up items not yet in the inventory of the user.
	-- This has the side-effect of not allowing items to be used before
	-- taken out of containers.
	local inv = Inventory:find{object = self}
	if not inv then
		return Actions:move_from_world_to_inv(user, self.id, user.id, 0)
	end
	local _,slot = inv:find_object{object = self}
	if inv.owner ~= user then
		return Actions:move_from_inv_to_inv(user, inv.owner.id, slot, user.id, 0)
	end
	-- Perform a type specific action.
	-- These are actions that can only be performed to inventory items.
	if self.spec.categories["potion"] then
		local types =
		{
			["health potion"] = { skill = "health", value = 30 },
			["mana potion"] = { skill = "willpower", value = 30 },
		}
		local type = types[self.name]
		if not type then return end
		local skills = user.skills
		if not skills then return end
		local value = skills:get_value{skill = type.skill}
		if not value then return end
		skills:set_value{skill = type.skill, value = value + type.value}
		Effect:play{effect = "impact1", object = user}
		self:subtract{count = 1}
		user:add_item{object = Item{spec = Itemspec:find{name = "empty bottle"}}}
	elseif self.spec.equipment_slot then
		if type(slot) == "string" then
			-- Unequip items in equipment slots.
			local dstslot = user.inventory:get_empty_slot()
			if not dstslot then return end
			Actions:move_from_inv_to_inv(user, inv.owner.id, slot, user.id, dstslot)
		else
			-- Equip items in inventory slots.
			Actions:move_from_inv_to_inv(user, inv.owner.id, slot, user.id, self.spec.equipment_slot)
		end
	end
end

--- Called when the item is being equipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.equipped = function(self, user, slot)
end

--- Called when the item is being unequipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.unequipped = function(self, user, slot)
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
Item.update_environment = function(self, secs)
	-- Environment scan and stuck handling.
	local ret,env = Object.update_environment(self, secs)
	if not ret or not env then return ret, env end
	-- Liquid physics.
	local liquid = env.liquid / env.total
	local magma = env.magma / env.total
	if liquid ~= (self.submerged or 0) then
		self.submerged = liquid > 0 and liquid or nil
		self.gravity = Config.gravity * (1 - liquid) + self.spec.water_gravity * liquid
	end
	-- Apply liquid friction.
	-- FIXME: Framerate dependent.
	if self.submerged then
		local damp = self.submerged * self.spec.water_friction * secs
		self.velocity = self.velocity - self.velocity * damp
	end
	return true, res
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Item.write = function(self)
	return string.format("local self=Item%s\n%s%s", serialize{
		angular = self.angular,
		count = self.count,
		id = self.id,
		looted = self.looted,
		spec = self.spec.name,
		position = self.position,
		rotation = self.rotation},
		Serialize:encode_inventory(self.inventory),
		"return self")
end

-- These take care of checking that items don't fall through ground.
Item.update_list = {}
Item.update_timer = Timer{delay = 0.5, func = function(timer, secs)
	local kept = {}
	for k,v in pairs(Item.update_list) do
		if v.realized then kept[k] = v end
		v.update_timer = v.update_timer + secs
		if v.update_timer > 1.0 then
			v.update_timer = 0.1 * math.random()
			v:update_environment(secs)
		end
	end
	Item.update_list = kept
	setmetatable(Item.update_list, {__mode = "v"})
end}
