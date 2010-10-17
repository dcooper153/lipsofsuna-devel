Mover = Class(Object)

--- Creates a mover.
-- @param clss Mover class.
-- @param args Arguments.<ul>
--   <li>target: Target translation.</li>
--   <li>speed: Movement speed.</li></ul>
-- @return New moving door.
Mover.new = function(clss, args)
	local self = Object(args)
	local target = args.target or Vector(0, 5, 0)
	self.class = clss
	self.target = nil
	self.static = true
	self.state = args.state or 0
	self.speed = args.speed or 0.2
	self.point1 = self.position - target * self.state
	self.point2 = self.position + target * (1 - self.state)
	return self
end

--- Activates the mover.
-- @param self Mover.
Mover.activate = function(self)
	if self.busy then return end
	self.busy = Thread(function()
		local done = false
		local diff = (self.state == 0 and 1 or -1) * self.speed
		while not done do
			self.state = self.state + diff * coroutine.yield()
			self.state = math.max(0, math.min(1, self.state))
			done = self.state == 0 or self.state == 1
			self.position = self.point2 * self.state + self.point1 * (1 - self.state)
		end
		self.busy = false
	end)
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Mover.use_cb = function(self, user)
	if not Quest:start{object = self, user = user} then
		self:activate()
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Mover.write = function(self)
	return string.gsub(Object.write(self), "}\n",
		",self.state=" .. serialize_value(math.ceil(self.state)) ..
		",self.point1=" .. serialize_vector(self.point1) ..
		",self.point2=" .. serialize_vector(self.point2) .. "}\n")
end
