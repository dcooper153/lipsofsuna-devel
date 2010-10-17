Decayer = Class(Object)

--- Creates a new decaying object.
-- @param self Decayer class.
-- @param args Arguments.
-- @return New decaying object.
Decayer.new = function(clss, args)
	local self = Object.new(clss, args)
	self.model = args and args.model or "meat-000"
	self.physics = args and args.physics or "rigid"
	self.timer = 5.0

	Thread(function()
		local t = 0
		while t < self.timer do t = t + coroutine.yield() end
		Particles:create(self.position, "evaporate")
		self.realized = false
		self:purge()
	end)

	return self
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Decayer.write = function(self)
	return string.gsub(Object.write(self), "}\n",
		",timer=" .. serialize_value(self.timer) .. "}\n")
end
