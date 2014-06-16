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
local ObjectChunkManager = require("core/objects/object-chunk-manager")

--- Manages the objects of the game.
-- @type ObjectManager
local ObjectManager = Class("ObjectManager")

--- Creates a new object manager.
-- @param clss ObjectManager class.
-- @param chunk_size Chunk size.
-- @param grid_size Grid size.
-- @return ObjectManager.
ObjectManager.new = function(clss, chunk_size, grid_size)
	local self = Class.new(clss)
	-- Initialize the object tables.
	self.objects_by_id = setmetatable({}, {__mode = "v"})
	self.visible_by_id = {}
	-- Initialize the hooks.
	self.object_created_hooks = Hooks()
	self.object_detached_hooks = Hooks()
	self.object_update_hooks = Hooks()
	-- Initialize the sector manager.
	self.chunks = ObjectChunkManager(self, chunk_size, grid_size)
	-- TODO: These should be registered elsewhere.
	self.__classes = {
		Actor = require("core/objects/actor"),
		AreaSpell = require("core/objects/areaspell"),
		Item = require("core/objects/item"),
		MissileSpell = require("core/objects/missilespell"),
		Obstacle = require("core/objects/obstacle"),
		Player = require("core/objects/player"),
		Static = require("core/objects/static")}
	self.__specs = {
		Actor = "ActorSpec",
		AreaSpell = "SpellSpec",
		Item = "ItemSpec",
		MissileSpell = "SpellSpec",
		Obstacle = "ObstacleSpec",
		Player = "ActorSpec",
		Static = "StaticSpec"}
	return self
end

--- Adds an object to the manager.
-- @param self ObjectManager.
-- @param object Object.
ObjectManager.add = function(self, object)
	self.objects_by_id[object:get_id()] = object
end

--- Creates a new object.
-- @param self ObjectManager.
-- @param clss Object class name.
-- @param id Object ID. Nil for automatic.
-- @return Object.
ObjectManager.create_object = function(self, clss, id)
	local c = self.__classes[clss]
	return c(self, id)
end

--- Creates a new object.
-- @param self ObjectManager.
-- @param clss Object class name.
-- @param spec Spec name.
-- @param id Object ID. Nil for automatic.
-- @return Object on success. Nil otherwise.
ObjectManager.create_object_by_spec = function(self, clss, spec, id)
	local s = Main.specs:find_by_name(self.__specs[clss], spec)
	if not s then return end
	local c = self.__classes[clss]
	local o = c(self, id)
	o:set_spec(s)
	return o
end

--- Creates a new object.
-- @param self ObjectManager.
-- @param clss Object class name.
-- @param cat Spec category.
-- @param id Object ID. Nil for automatic.
-- @return Object on success. Nil otherwise.
ObjectManager.create_object_by_spec_category = function(self, clss, cat, id)
	local s = Main.specs:find_by_category(self.__specs[clss], cat)
	if not s then return end
	local c = self.__classes[clss]
	local o = c(self, id)
	o:set_spec(s[math.random(#s)])
	return o
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
-- @return Dictionary of objects.
ObjectManager.find_by_point = function(self, point, radius)
	local dict = {}
	local list = Los.object_find{point = point.handle, radius = radius}
	for k,v in pairs(list) do
		local o = self.objects_by_id[v]
		if o then
			dict[o:get_id()] = o
		end
	end
	return dict
end

--- Finds objects in the given sector.
-- @param self ObjectManager.
-- @param sector Sector ID.
-- @return Dictionary of objects.
ObjectManager.find_by_sector = function(self, sector)
	local dict = {}
	for k,v in pairs(self.visible_by_id) do
		if v:get_sector() == sector then
			dict[v:get_id()] = v
		end
	end
	return dict
end

--- Loads a chunk.
-- @param self ObjectManager.
-- @param id Chunk ID.
ObjectManager.load_chunk = function(self, id)
	local x,z = self.chunks:get_chunk_xz_by_id(id)
	self.chunks:load_chunk(x, z)
end

--- Marks an object as visible or hidden.<br/>
--
-- Hidden objects do not consume any time during the update cycle so properly
-- showing and hiding objects can improve performance significantly when
-- there are, for example, lots of inventory items.
--
-- @param self ObjectManager.
-- @param object Object.
-- @param value True for visible, false for hidden.
ObjectManager.mark_visible = function(self, object, value)
	if value then
		self.visible_by_id[object:get_id()] = object
	else
		self.visible_by_id[object:get_id()] = nil
	end
end

--- Unloads all the objects.
-- @param self ObjectManager.
ObjectManager.unload_all = function(self)
	-- Unload the objects.
	for k,v in pairs(self.visible_by_id) do
		v:detach()
	end
	self.objects_by_id = setmetatable({}, {__mode = "v"})
	self.visible_by_id = {}
	-- Unload the chunks.
	self.chunks:unload_all_chunks()
end

--- Refreshes objects.
-- @param self ObjectManager.
-- @param point Point.
-- @param radius Radius.
ObjectManager.refresh_point = function(self, point, radius)
	self.chunks:refresh(point, radius)
end

--- Saves all active sectors to the database.
-- @param self ObjectManager.
ObjectManager.save_world = function(self)
	self.chunks:save_world()
end

--- Updates active objects.
-- @param self ObjectManager.
-- @param secs Seconds since the last update.
ObjectManager.update = function(self, secs)
	self.chunks:update(secs)
	Main.timing:start_action("objectS")
	for k,v in pairs(self.visible_by_id) do
		if v:has_server_data() then v:update_server(secs) end
	end
	Main.timing:start_action("objectC")
	for k,v in pairs(self.visible_by_id) do
		if v:has_client_data() then v:update_client(secs) end
	end
	Main.timing:start_action("objectG")
	for k,v in pairs(self.visible_by_id) do
		v:update(secs)
	end
end

--- Sets the database used by the manager.
-- @param self ObjectManager.
-- @param value Database to set. Nil to unset.
ObjectManager.set_database = function(self, value)
	if value then
		value:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);")
	end
	self.chunks.database = value
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

--- Toggles automatic chunk unloading.
-- @param self ObjectManager.
-- @param value True to enable. False to disable.
ObjectManager.set_unloading = function(self, value)
	self.chunks.enable_unloading = value
	self.chunks.unload_time = value and 10
end

--- Gets the dictionary of visible objects.
-- @param self ObjectManager.
-- @return Dictionary.
ObjectManager.get_visible_objects = function(self)
	return self.visible_by_id
end

return ObjectManager
