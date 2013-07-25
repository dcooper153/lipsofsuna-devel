--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.obstacle
-- @alias Obstacle

local Class = require("system/class")
local Item = require("core/objects/item")
local ObjectSerializer = require("core/objects/object-serializer")
local SimulationObject = require("core/objects/simulation")

--- TODO:doc
-- @type Obstacle
local Obstacle = Class("Obstacle", SimulationObject)
Obstacle.serializer = ObjectSerializer(
{
	"base",
	"fields",
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
		name = "health",
		type = "number"
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

--- Creates an obstacle.
-- @param clss Mover class.
-- @param manager Object manager.
-- @param id Unique object ID. Nil for a random free one.
-- @return New obstacle.
Obstacle.new = function(clss, manager, id)
	local self = SimulationObject.new(clss, manager, id)
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Obstacle.clone = function(self)
	-- TODO: Clone dialog variables?
	local o = Obstacle(self.manager)
	o:set_spec(self.spec)
	o:set_position(self:get_position())
	o:set_rotation(self:get_rotation())
	o.health = self.health
	o.physics:set_angular(self.physics:get_angular())
	return o
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
Obstacle.damaged = function(self, args)
	-- Obstacles whose spec has no health are indestructible.
	if not self.spec.health then return end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - args.amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Obstacle.die = function(self)
	for k,v in ipairs(self.spec.destroy_items) do
		local spec = Itemspec:find{name = v[1]}
		if spec then
			local p = self:transform_local_to_global(v[2])
			local r = self:get_rotation() * (v[3] or Quaternion())
			local item = Item(self.manager)
			item:set_spec(spec)
			item:set_position(p)
			item:set_rotation(r)
			item:randomize()
			item:set_visible(true)
		end
	end
	SimulationObject.die(self)
end

Obstacle.set_visible = function(self, value)
	SimulationObject.set_visible(self, value)
	-- Initialize physics constraints.
	if value and self:has_server_data() then
		local cons = self.spec:get_constraints()
		if cons then
			for k,v in pairs(cons) do
				if v.constraint == "hinge" then
					self.physics:insert_hinge_constraint{position = v.offset, axis = v.axis}
				end
			end
		end
	end
end

Obstacle.set_spec = function(self, value)
	local spec = type(value) == "string" and Obstaclespec:find{name = value} or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	-- Configure physics.
	self.physics:set_collision_group(spec.collision_group)
	self.physics:set_collision_mask(spec.collision_mask)
	self.physics:set_gravity(spec.gravity)
	self.physics:set_mass(spec.mass)
	self.physics:set_physics(spec.physics)
	-- Create the marker.
	if self:has_server_data() and spec.marker then
		self.marker = Main.markers:create(spec.marker, self:get_id(), self:get_position())
	end
	-- Set the model.
	self:set_model_name(spec.model)
end

Obstacle.get_storage_type = function(self)
	return "obstacle"
end

return Obstacle
