require "server/objects/object"

Obstacle = Class(Object)
Obstacle.class_name = "Obstacle"

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
	copy("health")
	copy("name")
	copy("rotation")
	copy("position")
	copy("spec")
	copy("realized")
	copy("variables")
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Obstacle.clone = function(self)
	local variables = {}
	for k,v in pairs(self.variables) do variables[k] = v end
	return Obstacle{
		angular = self.angular,
		health = self.health,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec,
		variables = variables}
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
			local p = self.position + self.rotation * (v[2] or Vector())
			local r = self.rotation * (v[3] or Quaternion())
			local o = Item{random = true, spec = spec, position = p, rotation = r, realized = true}
		end
	end
	Object.die(self)
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
		user.inventory:merge_or_drop_object(item)
		user:send{packet = Packet(packets.MESSAGE, "string", "Harvested " .. item.name .. ".")}
		-- Harvesting behavior.
		if self.spec.harvest_behavior == "destroy" then
			self:die()
		end
	end
	Object.use_cb(self, user)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
Obstacle.write_db = function(self, db)
	-- Write the object.
	local data = string.format("return Obstacle%s", serialize{
		angular = self.angular,
		health = self.health,
		id = self.id,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name,
		variables = self.variables})
	db:query([[REPLACE INTO object_data (id,type,data) VALUES (?,?,?);]], {self.id, "obstacle", data})
	-- Write the sector.
	if self.sector then
		db:query([[REPLACE INTO object_sectors (id,sector) VALUES (?,?);]],
			{self.id, self.sector})
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {self.id})
	end
end

Obstacle:add_setters{
	realized = function(self, value)
		Object.setters.realized(self, value)
		if value then
			local cons = self.spec:get_constraints()
			if cons then
				for k,v in pairs(cons) do
					if v.constraint == "hinge" then
						self:insert_hinge_constraint{position = v.offset, axis = v.axis}
					end
				end
			end
		end
	end,
	spec = function(self, value)
		local spec = type(value) == "string" and Obstaclespec:find{name = value} or value
		if not spec then return end
		rawset(self, "spec", spec)
		self.collision_group = spec.collision_group
		self.collision_mask = spec.collision_mask
		self.model = spec.model
		self.mass = spec.mass
		self.gravity = spec.gravity
		self.physics = spec.physics
		if spec.marker then
			self.marker = Marker{name = spec.marker, position = self.position, target = self.id}
		end
	end}
