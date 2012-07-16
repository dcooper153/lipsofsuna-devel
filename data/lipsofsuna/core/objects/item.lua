require(Mod.path .. "simulation")

Item = Class(SimulationObject)
Item.class_name = "Item"
Item.pickable = true

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
	local self = SimulationObject.new(clss, {id = args.id})
	if args then
		if args.angular then self.angular = args.angular end
		if args.count then self.count = args.count end
		if args.looted then self.looted = args.looted end
		if args.position then self:set_position(args.position) end
		if args.random then self.random = args.random end
		if args.rotation then self:set_rotation(args.rotation) end
		if args.spec then self:set_spec(args.spec) end
		if args.realized then self:set_visible(args.realized) end
	end
	self.update_timer = 0.3 * math.random()
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

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Item.contact_cb = function(self, result)
	-- Multiple contact points might be generated during one frame so we
	-- might have to skip some of the calls.
	if not self.contact_args then return end
	if self.spec.categories["boomerang"] then
		-- Boomerang mode.
		if result.object == self.contact_args.owner then
			-- Owner catch.
			if self.contact_args.owner.inventory:merge_object(self) then
				local o = self.contact_args.owner:get_weapon()
				if not o then self.contact_args.owner:set_weapon(self) end
			end
		else
			-- Damage target.
			self.contact_args.feat:apply{
				charge = self.contact_args.charge,
				object = result.object,
				owner = self.contact_args.owner,
				point = result.point,
				tile = result.tile,
				weapon = self}
		end
		-- Disable boomerang mode.
		if self.timer then
			self.timer:disable()
			self.timer = nil
		end
		self.contact_events = false
		self.contact_args = nil
		self.gravity = self.spec.gravity
		self:animate("fly stop")
	else
		-- Projectile mode.
		if SimulationObject.contact_cb(self, result) then
			self.gravity = self.spec.gravity
		end
	end
end

--- Gets the armor class of the item.
-- @param self Object.
-- @param user Actor.
-- @return Armor rating.
Item.get_armor_class = function(self, user)
	return self.spec.armor_class
end

--- Gets the weapon damage types of the item.
-- @param self Object.
-- @param user Actor.
-- @return Array of influences.
Item.get_weapon_influences = function(self, user)
	if not self.spec.influences then return {} end
	-- Calculate the damage multiplier.
	local mult = 1
	if user.skills then
		mult = user.skills:calculate_damage_multiplier_for_item(self)
	end
	-- Calculate influences.
	local influences = {}
	for k,v in pairs(self.spec.influences) do
		influences[k] = mult * v
	end
	return influences
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
-- @return True if not broken yet.
Item.damaged = function(self, args)
	-- Items whose spec has no health are indestructible.
	if not self.spec.health then return true end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - args.amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
		return
	end
	return true
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
			Utils:explosion(self.position)
		end
	end
	-- Remove from the world.
	SimulationObject.die(self)
end

--- Splits items from the stack.
-- @param self Object.
-- @param count Number of items to split.
-- @return Object.
Item.split = function(self, count)
	local c = count or 1
	if c < self.count then
		local o = self:clone()
		self:subtract(c)
		o.count = c
		return o
	else
		self:detach()
		return self
	end
end

--- Fires or throws the item.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
-- @return The split and fired item.
Item.fire = function(self, args)
	-- Split a projectile from the stack and fire it.
	if not args.owner or not args.feat then return end
	local proj = self:split()
	SimulationObject.fire(proj, args)
	proj.gravity = self.spec.gravity_projectile
	-- Special handling for boomerangs.
	if proj.spec.categories["boomerang"] then
		-- Work around an initial collision with the user.
		proj:set_position(proj:get_position() + proj:get_rotation() * Vector(0,0,-0.7))
		-- Enable boomerang return.
		proj.state = 0
		proj.rotation = Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi}
		proj:animate("fly start")
		proj.timer = Timer{delay = 1, func = function(timer, secs)
			if proj.state == 0 then
				proj.velocity = proj.velocity * -2
				proj.state = 1
			elseif proj.state == 1 then
				proj.velocity = proj.velocity * 2
				proj.state = 2
			else
				proj.timer = nil
				proj.gravity = proj.spec.gravity
				timer:disable()
			end
		end}
	end
	-- Return the fired projectile.
	return proj
end

--- Called when the object is examined.
-- @param self Object.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send_message(self.spec.name)
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

--- Writes the object to a string.
-- @param self Object.
-- @return Data string.
Item.write = function(self)
	return string.format("local self=Item%s\n%s", serialize{
		angular = self.angular,
		count = self.count,
		id = self.id,
		looted = self.looted,
		spec = self.spec.name,
		position = self.position,
		rotation = self.rotation},
		"return self")
end

--- Reads the object from a database.
-- @param self Object.
-- @param db Database.
Item.read_db = function(self, db)
	Server.serialize:load_object_inventory(self)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
Item.write_db = function(self, db)
	-- Write the object.
	local data = serialize{
		angular = self.angular,
		count = self.count,
		looted = self.looted,
		position = self.position,
		rotation = self.rotation}
	db:query([[REPLACE INTO object_data (id,type,spec,dead,data) VALUES (?,?,?,?,?);]],
		{self.id, "item", self.spec.name, 0, data})
	-- Write the sector.
	if self.sector then
		if self.spec.important then
			db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {self.id, self.sector, nil})
		else
			db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {self.id, self.sector, 0})
		end
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {self.id})
	end
	-- Write the inventory contents.
	db:query([[DELETE FROM object_inventory WHERE parent=?;]], {self.id})
	for index,object in pairs(self.inventory.stored) do
		object:write_db(db, index)
	end
	-- Write the own inventory index.
	local parent = self.parent and SimulationObject:find{id = self.parent}
	if parent then
		local index = parent.inventory:get_index_by_object(self)
		local slot = parent.inventory:get_slot_by_index(index)
		db:query([[REPLACE INTO object_inventory (id,parent,offset,slot) VALUES (?,?,?,?);]],
			{self.id, self.parent, index, slot})
	else
		db:query([[DELETE FROM object_inventory WHERE id=?;]], {self.id})
	end
end

Item.set_spec = function(self, value)
	local spec = type(value) == "string" and Itemspec:find{name = value} or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	-- Configure physics.
	if self:has_server_data() then
		-- FIXME: Why does client side picking break if this is set by
		-- the client when connected to a remote server?
		self.physics = "rigid"
		self.mass = spec.mass
	end
	self.collision_group = spec.collision_group
	self.collision_mask = spec.collision_mask
	self.friction_liquid = spec.water_friction
	self.gravity = spec.gravity
	self.gravity_liquid = spec.water_gravity
	-- Create the inventory.
	if spec.inventory_size and not self.inventory then
		self.inventory = Inventory{id = self.id, size = spec.inventory_size}
	end
	-- Create server data.
	if self:has_server_data() then
		-- Create static loot.
		if self.inventory then
			for k,v in pairs(spec.inventory_items) do
				self.inventory:merge_object(Item{spec = Itemspec:find{name = v}})
			end
		end
		-- Create random loot.
		if self.random and self.inventory and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count_min or spec.loot_count_max then
				local min = spec.loot_count_min or 0
				local max = spec.loot_count_max or min
				num_item = math.random(min, max)
			else
				num_item = math.random(0, self.inventory.size)
			end
			for i = 1,num_item do
				local cat = spec.loot_categories[math.random(1, num_cat)]
				self.inventory:merge_object(Item{spec = Itemspec:random{category = cat}})
			end
		end
	end
	-- Set the model.
	self:set_model_name(spec.model)
end
