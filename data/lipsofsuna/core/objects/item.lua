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
local Json = require("system/json")
local ObjectSerializer = require("core/objects/object-serializer")
local SimulationObject = require("core/objects/simulation")

--- Item object.
-- @type Item
local Item = Class("Item", SimulationObject)
Item.serializer = ObjectSerializer(
{
	"base",
	"fields",
	"inventory",
	"parent",
	"sector"
},
{
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
		name = "important",
		type = "boolean",
		get = function(self) return self:get_important() end,
		set = function(self, v) self:set_important(v) end
	},
	{
		name = "looted",
		type = "boolean"
	},
	{
		name = "spell_action",
		type = "string"
	},
	{
		name = "spell_modifiers",
		type = "string",
		get = function(self)
			if not self.spell_modifiers then return end
			return Json:encode(self.spell_modifiers)
		end,
		set = function(self, v)
			if not v or v == "" then return end
			self.spell_modifiers = Json:decode(v)
		end
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
})

--- Creates an item.
-- @param clss Item class.
-- @param manager Object manager.
-- @param id Unique object ID. Nil for a random free one.
-- @return Item.
Item.new = function(clss, manager, id)
	local self = SimulationObject.new(clss, manager, id)
	self.update_timer = 0.3 * math.random()
	return self
end

--- Creates a copy of the item.
-- @param self Item.
-- @return Object.
Item.clone = function(self)
	local o = Item(self.manager)
	o:set_spec(self:get_spec())
	o:set_position(self:get_position())
	o:set_rotation(self:get_rotation())
	o.physics:set_angular(self.physics:get_angular())
	return o
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

--- Randomizes the item.
-- @param self Item.
Item.randomize = function(self)
	-- Get the item spec.
	local spec = self:get_spec()
	if not spec then return end
	-- Create static loot.
	if self.inventory:get_size() > 0 then
		for k,v in pairs(spec.inventory_items) do
			local ispec = Itemspec:find_by_name(k)
			if ispec then
				local item = Item(self.manager)
				item:set_spec(ispec)
				item:set_count(v)
				self.inventory:merge_object(item)
			end
		end
	end
	-- Create random loot.
	if self.inventory:get_size() > 0 and spec.loot_categories then
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
			local item = Item(self.manager)
			item:set_spec(Itemspec:random{category = cat})
			self.inventory:merge_object(item)
		end
	end
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
Item.update_server = function(self, secs)
	if self.controller then
		self.controller:update(secs)
	end
end

--- Gets the stack count of the item.
-- @param self Item.
-- @return Count.
Item.get_count = function(self)
	return self.__count or 1
end

--- Sets the stack count of the item.
-- @param self Item.
-- @param value Count.
Item.set_count = function(self, value)
	-- Store the new count.
	if self.__count == value then return end
	self.__count = value ~= 0 and value or nil
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
	-- Set the model.
	self:set_model_name(spec.model)
end

Item.get_storage_type = function(self)
	return "item"
end

return Item
