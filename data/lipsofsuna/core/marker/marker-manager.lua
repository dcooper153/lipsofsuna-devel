--- Map marker manager.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module lipsofsuna.core.marker.marker_manager
-- @alias MarkerManager

local Class = require("system/class")
local Marker = require("core/marker/marker")

--- Map marker manager.
-- @type MarkerManager
local MarkerManager = Class("MarkerManager")

--- Creates a new map marker manager.
-- @param clss MarkerManager class.
-- @return MarkerManager.
MarkerManager.new = function(clss)
	local self = Class.new(clss)
	self.__dict_name = {}
	self.__dict_discoverable = {}
	return self
end

--- Creates a new map marker.
-- @param self MarkerManager.
-- @param name Marker name.
-- @param target ID of the target object.
-- @param position Position vector if known. Nil otherwise.
-- @param discoverable True to allow the marker to be automatically discovered. False otherwise.
-- @return Marker.
MarkerManager.create = function(self, name, target, position, discoverable)
	local marker = Marker{name = name, target = target, position = position, discoverable = discoverable}
	self.__dict_name[name] = marker
	if discoverable then
		self.__dict_discoverable[name] = marker
	end
	return marker
end

--- Finds a map marker.
-- @param self MarkerManager.
-- @param name Marker name.
-- @return Marker if found. Nil otherwise.
MarkerManager.find_by_name = function(self, name)
	return self.__dict_name[name]
end

--- Loads the markers from the database.
-- @param self MarkerManager.
-- @param db Database.
MarkerManager.load = function(self, db)
	local r = db:query("SELECT name,id,x,y,z,unlocked,discoverable FROM markers;")
	for k,v in ipairs(r) do
		local marker = Marker{name = v[1], target = v[2],
			position = Vector(v[3], v[4], v[5]),
			unlocked = (v[6] == 1), discoverable = (v[7] == 1)}
		self.__dict_name[marker.name] = marker
		if marker.discoverable then
			self.__dict_discoverable[marker.name] = marker
		end
	end
end

--- Resets the map marker system.
-- @param self MarkerManager.
MarkerManager.reset = function(self)
	for k,v in pairs(self.__dict_name) do
		v:disable()
	end
	self.__dict_name = {}
	self.__dict_discoverable = {}
end

--- Saves the markers into the database.
-- @param self MarkerManager.
-- @param db Database.
-- @param erase True to reset the tables. False otherwise.
MarkerManager.save = function(self, db, erase)
	db:query("BEGIN TRANSACTION;")
	if erase then
		db:query([[DROP TABLE IF EXISTS markers;]])
		db:query([[CREATE TABLE markers (
			name TEXT PRIMARY KEY,
			id INTEGER,
			x FLOAT,
			y FLOAT,
			z FLOAT,
			unlocked INTENGER,
			discoverable INTEGER);]])
	end
	for k,v in pairs(self.__dict_name) do
		db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
			{k, v.target, v.position.x, v.position.y, v.position.z,
			 v.unlocked and 1 or 0, v.discoverable and 1 or 0})
	end
	db:query("END TRANSACTION;")
end

return MarkerManager
