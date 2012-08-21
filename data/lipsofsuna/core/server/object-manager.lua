local Class = require("system/class")

local ObjectManager = Class("ObjectManager")

--- Creates a new object manager.
-- @param clss ObjectManager class.
-- @return ObjectManager.
ObjectManager.new = function(clss)
	local self = Class.new(clss)
	self.objects_by_id = setmetatable({}, {__mode = "v"})
	return self
end

--- Adds an object to the manager.
-- @param self ObjectManager.
-- @param object.
ObjectManager.add = function(self, object)
	self.objects_by_id[object:get_id()] = object
end

--- Detaches all objects.
-- @param self ObjectManager.
ObjectManager.detach_all = function(self)
	for k,v in pairs(self.objects_by_id) do
		v:detach()
		self.objects_by_id[k] = nil
	end
end

--- Finds an object by its ID.
-- @param self ObjectManager.
-- @param id Object ID.
-- @return Object, or nil.
ObjectManager.find_by_id = function(self, id)
	return self.objects_by_id[id]
end

--- Finds an object by its ID and check that it's near the given point.
-- @param self ObjectManager.
-- @param id Object ID.
-- @param point Center point for radius search.
-- @param radius Search radius for radius search.
-- @return Object, or nil.
ObjectManager.find_by_id_and_point = function(self, id, point, radius)
	-- Search by the ID.
	local obj = self.objects_by_id[id]
	if not obj then return end
	-- Check for the distance.
	if not obj:get_visible() then return end
	if (obj.position - point).length > radius then return end
	return obj
end

--- Finds objects near the given point.
-- @param self ObjectManager.
-- @param point Center point for radius search.
-- @param radius Search radius for radius search.
-- @param sector Sector ID, or nil for all sectors.
-- @return Dictionary of objects.
ObjectManager.find_by_point = function(self, point, radius, sector)
	local dict = {}
	local list = Los.object_find{point = point.handle, radius = radius, sector = sector}
	for k,v in pairs(list) do
		local o = __userdata_lookup[v]
		dict[o:get_id()] = o
	end
	return dict
end

--- Finds objects in the given sector.
-- @param self ObjectManager.
-- @param sector Sector ID.
-- @return Dictionary of objects.
ObjectManager.find_by_sector = function(self, sector)
	local dict = {}
	local list = Los.object_find{sector = sector}
	for k,v in pairs(list) do
		local o = __userdata_lookup[v]
		dict[o:get_id()] = o
	end
	return dict
end

--- Gets a free object ID.
-- @param self ObjectManager.
-- @return Free object ID.
ObjectManager.get_free_id = function(self)
	if Server.initialized then
		while true do
			local id = math.random(0x0000001, 0x0FFFFFF)
			if not self:find_by_id(id) then
				if not Server.object_database:does_object_exist(id) then
					return id
				end
			end
		end
	else
		while true do
			local id = math.random(0x1000000, 0x7FFFFFF)
			if not self:find_by_id(id) then
				return id
			end
		end
	end
end

return ObjectManager
