--- Manages the objects of the game.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.object_manager
-- @alias ObjectManager

local Class = require("system/class")
local Hooks = require("system/hooks")

--- Manages the objects of the game.
-- @type ObjectManager
local ObjectManager = Class("ObjectManager")

--- Creates a new object manager.
-- @param clss ObjectManager class.
-- @return ObjectManager.
ObjectManager.new = function(clss)
	local self = Class.new(clss)
	-- Initialize the object tables.
	self.active_by_id = setmetatable({}, {__mode = "v"})
	self.objects_by_id = setmetatable({}, {__mode = "v"})
	-- Initialize the hooks.
	self.object_created_hooks = Hooks()
	return self
end

--- Marks an object as active or inactive.<br/>
--
-- Inactive objects do not consume any time during the update cycle so
-- properly activating and deactivating objects can improve performance
-- significantly when there are lots of inventory items or other objects
-- that do not require processing.
--
-- @param self ObjectManager.
-- @param object Object.
-- @param value True for active, false for inactive.
ObjectManager.activate_object = function(self, object, value)
	if value then
		self.active_by_id[object:get_id()] = object
	else
		self.active_by_id[object:get_id()] = nil
	end
end

--- Adds an object to the manager.
-- @param self ObjectManager.
-- @param object Object.
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
	self.active_by_id = {}
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
	if list then
		for k,v in pairs(list) do
			local o = __userdata_lookup[v]
			dict[o:get_id()] = o
		end
	end
	return dict
end

--- Updates active objects.
-- @param self ObjectManager.
-- @param secs Seconds since the last update.
ObjectManager.update = function(self, secs)
	-- FIXME: Updates all objects due to terrain and object loading not being
	-- in sync currently. The update is required by SimulationObject.update()
	-- to freeze object physics until the terrain has been loaded.
	for k,v in pairs(self.objects_by_id) do
	--for k,v in pairs(self.active_by_id) do
		v:update(secs)
	end
end

--- Gets the dictionary of active objects.
-- @param self ObjectManager.
-- @return Dictionary.
ObjectManager.get_active_objects = function(self)
	return self.active_by_id
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
