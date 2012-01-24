require "server/objects/object"

Staticobject = Class(Object)
Staticobject.class_name = "Staticobject"

Staticobject:add_setters{
	spec = function(self, value)
		local spec = type(value) == "string" and Staticspec:find{name = value} or value
		if not spec then return end
		rawset(self, "spec", spec)
		self.collision_group = spec.collision_group
		self.collision_mask = spec.collision_mask
		self.model = spec.model
		self.gravity = spec.gravity
		self.physics = "static"
		self.position = spec.position
		self.rotation = spec.rotation
	end}

--- Creates an static object.
-- @param clss Staticobject class.
-- @param args Arguments.
-- @return New static object.
Staticobject.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("rotation")
	copy("position")
	copy("spec")
	copy("realized")
	copy("variables")
	self.disable_unloading = true
	self.disable_saving = true
	return self
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

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Staticobject.write = function(self)
end
