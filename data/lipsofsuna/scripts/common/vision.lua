Vision = Class()
Vision.listeners = {}

--- Creates a new vision listener.
-- @param clss Vision class.
-- @param args Arguments.
-- @return New vision listener.
Vision.new = function(clss, args)
	local self = Class.new(clss, args)
	self.objects = {}
	setmetatable(self.objects, {__mode = "k"})
	self:enable()
	return self
end

--- Enables the vision listener.
-- @param self Vision.
Vision.enable = function(self)
	-- Select unique ID.
	self.id = 0
	while self.id == 0 do
		self.id = math.random()
		if Vision.listeners[id] then self.id = 0 end
	end
	-- Add to dictionary.
	Vision.listeners[self.id] = self
end

--- Disables the vision listener.
-- @param self Vision.
Vision.disable = function(self)
	Vision.listeners[self.id] = nil
end

--- Handles a vision event.
-- @param self Vision.
-- @param args Arguments.
Vision.handle_event = function(self, args)
	local p = args.point or args.object.position
	local l = self.point or self.object.position
	local r = self.radius or 10.0
	local d = (p - l).length
	-- Maintain the list of seen objects.
	if args.object then
		if args.type == "object-shown" then
			-- Add explicitly shown objects.
			if self.objects[args.object] then return end
			self.objects[args.object] = true
		elseif args.type == "object-hidden" then
			-- Remove explicitly hidden objects.
			if not self.objects[args.object] then return end
			self.objects[args.object] = nil
		elseif args.type == "object-moved" then
			-- Add objects that move inside the vision radius.
			-- Remove objects that move outside of the vision radius.
			if d < r and not self.objects[args.object] then
				self.objects[args.object] = true
				self.callback{type = "object-shown", object = args.object}
			elseif d >= r and self.objects[args.object] then
				self.objects[args.object] = nil
				self.callback{type = "object-hidden", object = args.object}
			end
		elseif d <= r then
			-- Add objects that emitted a vision event before we had a
			-- chance to detect their appearance in a rescan.
			if not self.objects[args.object] then
				self.objects[args.object] = true
				self.callback{type = "object-shown", object = args.object}
			end
		end
	end
	-- Call the user callback.
	if d < r then self.callback(args) end
end

--- Rescans the area around the listener for visible objects.
-- @param self Vision.
Vision.rescan_objects = function(self)
	-- Find visible objects.
	local l = self.point or self.object.position
	local r = self.radius or 10.0
	local seeing = Object:find{point = l, radius = r}
	-- Add appearing objects.
	for k,v in pairs(seeing) do
		if not self.objects[v] then
			self.objects[v] = true
			self.callback{type = "object-shown", object = v}
		end
	end
	-- Remove disappearing objects.
	for k,v in pairs(self.objects) do
		if not seeing[k.id] then
			self.objects[k] = nil
			self.callback{type = "object-hidden", object = k}
		end
	end
end

Vision.event = function(clss, args)
	if args.id then args.object = Object:find{id = args.id} end
	if not args.point and not args.object then return end
	-- Update each listener.
	for k,v in pairs(clss.listeners) do
		if not args.restrict or args.restrict == v.object then
			if v.point or (v.object and v.object.realized) then
				if not args.object or args.object.realized or args.type == "object-hidden" then
					if v.callback then v:handle_event(args) end
				end
			end
		end
	end
end
