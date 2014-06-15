--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.static
-- @alias Staticobject

local Class = require("system/class")
local ObjectSerializer = require("core/objects/object-serializer")
local SimulationObject = require("core/objects/simulation")

--- TODO:doc
-- @type Staticobject
local Staticobject = Class("Staticobject", SimulationObject)
Staticobject.serializer = ObjectSerializer(
{
	"base",
	"fields"
},
{
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

--- Creates an static object.
-- @param clss Staticobject class.
-- @param manager Object manager.
-- @param args Arguments.
-- @return New static object.
Staticobject.new = function(clss, manager, args)
	local self = SimulationObject.new(clss, manager, args and args.id)
	self:set_static(true)
	if args then
		if args.position then self:set_position(args.position) end
		if args.rotation then self:set_rotation(args.rotation) end
		if args.spec then self:set_spec(args.spec) end
		if args.realized then self:set_visible(args.realized) end
	end
	Main.game.static_objects_by_id[self:get_id()] = self
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Staticobject.clone = function(self)
	local o = self.manager:create_object("Static")
	o:set_spec(self:get_spec())
	o:set_position(self:get_position())
	o:set_rotation(self:get_rotation())
	return o
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
	local spec = type(value) == "string" and Main.specs:find_by_name("StaticSpec", value) or value
	if not spec then return end
	SimulationObject.set_spec(self, spec)
	-- Configure physics.
	self.physics:set_collision_group(spec.collision_group)
	self.physics:set_collision_mask(spec.collision_mask)
	self.physics:set_gravity(spec.gravity)
	self.physics:set_physics("static")
	-- Create the marker.
	if spec.marker then
		self.marker = Main.markers:create(spec.marker, self:get_id(), self:get_position())
	end
	-- Set the model.
	self:set_model_name(spec.model)
end

Staticobject.get_storage_type = function(self)
	return "static"
end

return Staticobject
