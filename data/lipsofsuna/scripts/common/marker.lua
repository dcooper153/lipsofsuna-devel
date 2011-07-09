Marker = Class()
Marker.dict_name = {}

--- Finds a map marker.
-- @param clss Marker class.
-- @param args Arguments.<ul>
--   <li>name: Marker name.</li></ul>
-- @return Marker or nil.
Marker.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Creates a new map marker.
-- @param clss Marker class.
-- @param args Arguments.<ul>
--   <li>name: Marker name.</li>
--   <li>target: ID of the target object.</li></ul>
-- @return Marker.
Marker.new = function(clss, args)
	local self = Class.new(clss, args)
	clss.dict_name[args.name] = self
	return self
end

--- Resets the map marker system.
-- @param clss Marker class.
Marker.reset = function(clss)
	for k,v in pairs(clss.dict_name) do
		v:disable()
	end
	clss.dict_name = {}
end

--- Disables updates for the map marker.
-- @param self Marker.
Marker.disable = function(self)
	if self.timer then
		self.timer:disable()
		self.timer = nil
	end
end
