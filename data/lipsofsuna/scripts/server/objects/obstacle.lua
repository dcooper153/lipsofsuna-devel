Obstacle = Class(Object)

--- Creates an obstacle.
-- @param clss Mover class.
-- @param args Arguments.
-- @return New obstacle.
Obstacle.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("angular")
	copy("collision_group", 0x8000)
	copy("collision_mask", 0xFF)
	copy("health")
	copy("name")
	copy("rotation")
	copy("position")
	copy("spec")
	copy("realized")
	return self
end

--- Implements setters.
-- @param self Obstacle or Obstacle class.
-- @param key Key.
-- @param value Value.
Obstacle.setter = function(self, key, value)
	if key == "spec" then
		local spec = type(value) == "string" and Obstaclespec:find{name = value} or value
		if not spec then return end
		Object.setter(self, key, spec)
		self.model = spec.model
		self.mass = spec.mass
		self.gravity = spec.gravity
		self.physics = spec.physics
		if spec.marker then
			self.marker = Marker{name = spec.marker, position = self.position, target = self.id}
		end
	else
		Object.setter(self, key, value)
	end
end

--- Causes the obstacle to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Obstacle.damaged = function(self, amount)
	-- Obstacles whose spec has no health are indestructible.
	if not self.spec.health then return end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Obstacle.use_cb = function(self, user)
	if self.spec.harvest_enabled then
		-- Create list of harvestable items.
		local mats = {}
		for k,v in pairs(self.spec.harvest_materials) do table.insert(mats, k) end
		if #mats == 0 then return end
		-- Effect playback.
		if self.spec.harvest_effect then
			Effect:play{effect = self.spec.harvest_effect, point = self.position}
		end
		-- Choose a random item from the list.
		local item = Item{spec = Itemspec:find{name = mats[math.random(1, #mats)]}}
		user:add_item{object = item}
		user:send{packet = Packet(packets.MESSAGE, "string", "Harvested " .. item.name .. ".")}
		-- Harvesting behavior.
		if self.spec.harvest_behavior == "destroy" then
			self:die()
		end
	end
	Object.use_cb(self, user)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Obstacle.write = function(self)
	return "local self=Obstacle{" ..
		"angular=" .. serialize_value(self.angular) .. "," ..
		"health=" .. serialize_value(self.health) .. "," ..
		"id=" .. serialize_value(self.id) .. "," ..
		"position=" .. serialize_value(self.position) .. "," ..
		"rotation=" .. serialize_value(self.rotation) .. "," ..
		"spec=" .. serialize_value(self.spec.name) .. "}\n" ..
		"return self"
end
