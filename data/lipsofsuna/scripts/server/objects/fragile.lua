Fragile = Class(Object)

--- Creates a new fragile object.
-- @param self Fragile class.
-- @param args Arguments.
-- @return New fragile object.
Fragile.new = function(clss, args)
	local self = Object.new(clss, args)
	self.resistance = args and args.resistance or 5.0
	return self
end

Fragile.contact_cb = function(self, result)
	local damage = Damage:impulse(result.impulse)
	if damage > self.resistance then
		self:effect{effect = "thud-000"}
		self:damaged(damage)
		Particles:debug(result.point, "test")
	end
end

Fragile.damaged = function(self, amount)
	if self.resistance <= amount then
		self:die()
	end
end

Fragile.die = function(self)
	Voxel:erase{point = self.position} -- FIXME: Hardcoded to destroy terrain.
	Object.die(self)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Fragile.write = function(self)
	return string.gsub(Object.write(self), "}\n",
		",resistance=" .. serialize_value(self.resistance) .. "}\n")
end
