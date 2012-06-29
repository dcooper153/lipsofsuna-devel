require "server/objects/object"

Obstacle = Class(ServerObject)
Obstacle.class_name = "Obstacle"

--- Creates an obstacle.
-- @param clss Mover class.
-- @param args Arguments.
-- @return New obstacle.
Obstacle.new = function(clss, args)
	local self = ServerObject.new(clss, {id = args.id})
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
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Obstacle.clone = function(self)
	-- TODO: Clone dialog variables?
	return Obstacle{
		angular = self.angular,
		health = self.health,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec}
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
	ServerObject.die(self)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
Obstacle.write_db = function(self, db)
	-- Write the object.
	local data = serialize{
		angular = self.angular,
		health = self.health,
		position = self.position,
		rotation = self.rotation}
	db:query([[REPLACE INTO object_data (id,type,spec,dead,data) VALUES (?,?,?,?,?);]],
		{self.id, "obstacle", self.spec.name, 0, data})
	-- Write the sector.
	if self.sector then
		db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {self.id, self.sector, nil})
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {self.id})
	end
end

Obstacle:add_setters{
	realized = function(self, value)
		ServerObject.setters.realized(self, value)
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
