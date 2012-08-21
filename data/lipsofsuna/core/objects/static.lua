local Class = require("system/class")
local ObjectSerializer = require("core/server/object-serializer")
local SimulationObject = require("core/objects/simulation")

local Staticobject = Class("Staticobject", SimulationObject)
Staticobject.serializer = ObjectSerializer{
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

--- Creates an static object.
-- @param clss Staticobject class.
-- @param args Arguments.
-- @return New static object.
Staticobject.new = function(clss, args)
	local self = SimulationObject.new(clss, args and args.id)
	self:set_static(true)
	if args then
		if args.position then self:set_position(args.position) end
		if args.rotation then self:set_rotation(args.rotation) end
		if args.spec then self:set_spec(args.spec) end
		if args.realized then self:set_visible(args.realized) end
	end
	Game.static_objects_by_id[self:get_id()] = self
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Staticobject.clone = function(self)
	return Staticobject{
		position = self:get_position(),
		rotation = self:get_rotation(),
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

Staticobject.set_spec = function(self, value)
	local spec = type(value) == "string" and Staticspec:find{name = value} or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	self:set_collision_group(spec.collision_group)
	self:set_collision_mask(spec.collision_mask)
	self:set_gravity(spec.gravity)
	self:set_physics("static")
	-- Create the marker.
	if spec.marker then
		self.marker = Marker{name = spec.marker, position = self:get_position(), target = self:get_id()}
	end
	-- Set the model.
	self:set_model_name(spec.model)
end

Staticobject.get_storage_type = function(self)
	return "static"
end

Staticobject.get_storage_sector = function(self)
end

return Staticobject
