--- Item object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.item
-- @alias Item

local Class = require("system/class")
local ObjectSerializer = require("core/server/object-serializer")
local SimulationObject = require("core/objects/simulation")

--- Item object.
-- @type Item
local Item = Class("Item", SimulationObject)
Item.serializer = ObjectSerializer{
	{
		name = "angular",
		type = "vector",
		get = function(self) return self.physics:get_angular() end,
		set = function(self, v) return self.physics:set_angular(v) end
	},
	{
		name = "count",
		type = "number",
		get = function(self) return self:get_count() end,
		set = function(self, v) self:set_count(v) end
	},
	{
		name = "health",
		type = "number"
	},
	{
		name = "looted",
		type = "boolean"
	},
	{
		name = "position",
		type = "vector",
		get = function(self) return self:get_position() end,
		set = function(self, v) self:set_position(v) end
	},
	{
		name = "rotation",
		type = "quaternion",
		get = function(self) return self:get_rotation() end,
		set = function(self, v) self:set_rotation(v) end
	}
}

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
	local self = SimulationObject.new(clss, args and args.id)
	if args then
		if args.angular then self.physics:set_angular(args.angular) end
		if args.count then self:set_count(args.count) end
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
-- @param self Item.
-- @return Object.
Item.clone = function(self)
	return Item{
		spec = self.spec,
		angular = self.physics:get_angular(),
		position = self:get_position(),
		rotation = self:get_rotation()}
end

--- Handles physics contacts.
-- @param self Item.
-- @param result Contact result.
Item.contact_cb = function(self, result)
	if not self.controller then return end
	self.controller:handle_contact(result)
end

--- Causes the object to take damage.
-- @param self Item.
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
-- @param self Item.
Item.die = function(self)
	-- Mark as dead.
	if self.dead then return end
	self.dead = true
	-- Execute destruction actions.
	for k,v in ipairs(self.spec.destroy_actions) do
		if v == "explode" then
			Utils:explosion(self:get_position())
		end
	end
	-- Remove from the world.
	SimulationObject.die(self)
end

--- Called when the object is examined.
-- @param self Item.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send_message(self.spec.name)
end

--- Reads the object from a database.
-- @param self Item.
-- @param db Database.
Item.read_db = function(self, db)
	SimulationObject.read_db(self, db)
	Server.object_database:load_inventory(self)
end

--- Splits items from the stack.
-- @param self Item.
-- @param count Number of items to split.
-- @return Object.
Item.split = function(self, count)
	local c = count or 1
	if c < self:get_count() then
		local o = self:clone()
		self:subtract(c)
		o:set_count(c)
		return o
	else
		self:detach()
		return self
	end
end

--- Updates the state of the item.
-- @param self Item.
-- @param secs Seconds since the last update.
Item.update = function(self, secs)
	if self.controller then
		self.controller:update(secs)
	end
	SimulationObject.update(self, secs)
end

--- Writes the object to a database.
-- @param self Item.
-- @param db Database.
Item.write_db = function(self, db)
	-- Write the object data.
	local id = self:get_id()
	SimulationObject.write_db(self, db)
	-- Write the inventory contents.
	db:query([[DELETE FROM object_inventory WHERE parent=?;]], {id})
	for index,object in pairs(self.inventory.stored) do
		object:write_db(db)
	end
	-- Write the own inventory index.
	local parent = self.parent and Main.objects:find_by_id(self.parent)
	if parent then
		local index = parent.inventory:get_index_by_object(self)
		local slot = parent.inventory:get_slot_by_index(index)
		db:query([[REPLACE INTO object_inventory (id,parent,offset,slot) VALUES (?,?,?,?);]],
			{id, self.parent, index, slot})
	else
		db:query([[DELETE FROM object_inventory WHERE id=?;]], {id})
	end
end

--- Gets the armor class of the item.
-- @param self Item.
-- @param user Actor.
-- @return Armor rating.
Item.get_armor_class = function(self, user)
	return self.spec.armor_class
end

--- Gets the stack count of the item.
-- @param self Item.
-- @return Count.
Item.get_count = function(self)
	return self.__count or 1
end

--- Sets the stack count of the item.
-- @param self Item.
-- @param v Count.
Item.set_count = function(self, v)
	-- Store the new count.
	if self.__count == v then return end
	self.__count = v ~= 0 and v or nil
	-- Update the inventory containing the object.
	if self.parent then
		local parent = Main.objects:find_by_id(self.parent)
		if parent then
			parent.inventory:update_object(s)
		else
			self.parent = nil
		end
	end
end

--- Gets the weapon damage types of the item.
-- @param self Item.
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

--- Sets the itemspec of the object.
-- @param self Item.
-- @param value Itemspec.
Item.set_spec = function(self, value)
	local spec = type(value) == "string" and Itemspec:find{name = value} or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	-- Configure physics.
	if self:has_server_data() then
		-- FIXME: Why does client side picking break if this is set by
		-- the client when connected to a remote server?
		self.physics:set_physics("rigid")
		self.physics:set_mass(spec.mass)
	end
	self.physics:set_collision_group(spec.collision_group)
	self.physics:set_collision_mask(spec.collision_mask)
	self.physics:set_friction_liquid(spec.water_friction)
	self.physics:set_gravity(spec.gravity)
	self.physics:set_gravity_liquid(spec.water_gravity)
	-- Set the inventory size.
	self.inventory:set_size(spec.inventory_size)
	-- Create server data.
	if self:has_server_data() then
		-- Create static loot.
		if self.random and self.inventory:get_size() > 0 then
			for k,v in pairs(spec.inventory_items) do
				self.inventory:merge_object(Item{spec = Itemspec:find{name = v}})
			end
		end
		-- Create random loot.
		if self.random and self.inventory:get_size() > 0 and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count_min or spec.loot_count_max then
				local min = spec.loot_count_min or 0
				local max = spec.loot_count_max or min
				num_item = math.random(min, max)
			else
				num_item = math.random(0, self.inventory:get_size())
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

Item.get_storage_sector = function(self)
	return self:get_sector()
end

Item.get_storage_type = function(self)
	return "item"
end

return Item
