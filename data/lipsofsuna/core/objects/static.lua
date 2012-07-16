require(Mod.path .. "simulation")

Staticobject = Class(SimulationObject)
Staticobject.class_name = "Staticobject"

--- Creates an static object.
-- @param clss Staticobject class.
-- @param args Arguments.
-- @return New static object.
Staticobject.new = function(clss, args)
	local self = SimulationObject.new(clss, {id = args.id})
	self.disable_saving = true
	self.static = true
	if args then
		if args.position then self:set_position(args.position) end
		if args.rotation then self:set_rotation(args.rotation) end
		if args.spec then self:set_spec(args.spec) end
		if args.realized then self:set_visible(args.realized) end
	end
	Game.static_objects_by_id[self.id] = self
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Staticobject.clone = function(self)
	return Staticobject{
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
Staticobject.damaged = function(self, args)
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Staticobject.die = function(self)
end

--- Writes the obstacle to a database.
-- @param self Object.
-- @param db Database.
Staticobject.write_db = function(self, db)
	-- Write the object.
	local data = serialize{
		id = self.id,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name}
	db:query([[REPLACE INTO object_data (id,type,spec,dead,data) VALUES (?,?,?,?,?);]],
		{self.id, "static", self.spec.name, 0, data})
end

Staticobject.set_spec = function(self, value)
	local spec = type(value) == "string" and Staticspec:find{name = value} or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	self.collision_group = spec.collision_group
	self.collision_mask = spec.collision_mask
	self.gravity = spec.gravity
	self.physics = "static"
	-- Create the marker.
	if spec.marker then
		self.marker = Marker{name = spec.marker, position = self.position, target = self.id}
	end
	-- Set the model.
	self:set_model_name(spec.model)
end
