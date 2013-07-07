--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.serialize
-- @alias Serialize

local Class = require("system/class")
local Marker = require("core/marker")

--- TODO:doc
-- @type Serialize
local Serialize = Class("Serialize")
Serialize.game_version = "7"
Serialize.object_version = "7"

--- Creates a new serializer.
-- @param clss Serialize class.
-- @param db Database.
-- @return Serializer.
Serialize.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	self:init_game_database()
	return self
end

--- Loads everything except map data.
-- @param clss Serialize class.
Serialize.load = function(clss)
	clss:load_generator()
	clss:load_markers()
end

--- Loads map generator data from the database.
-- @param clss Serialize class.
Serialize.load_generator = function(clss)
	do return end --FIXME
	-- Load settings.
	local r1 = clss.db:query("SELECT key,value FROM generator_settings;")
	local f1 = {
		seed1 = function(v) Server.generator.seed1 = tonumber(v) end,
		seed2 = function(v) Server.generator.seed2 = tonumber(v) end,
		seed3 = function(v) Server.generator.seed3 = tonumber(v) end}
	for k,v in ipairs(r1) do
		local f = f1[v[1]]
		if f then f(v[2]) end
	end
	-- Load special sectors.
	local r2 = clss.db:query("SELECT id,value FROM generator_sectors;")
	for k,v in ipairs(r2) do
		Server.generator.sectors[v[1]] = v[2]
	end
end

--- Loads map markers from the database.
-- @param clss Serialize class.
Serialize.load_markers = function(clss)
	local r = clss.db:query("SELECT name,id,x,y,z,unlocked,discoverable FROM markers;")
	for k,v in ipairs(r) do
		Marker{name = v[1], target = v[2], position = Vector(v[3], v[4], v[5]),
			unlocked = (v[6] == 1), discoverable = (v[7] == 1)}
	end
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	if erase then Server.object_database:clear_objects() end
	Game.sectors:save_world(erase)
	Main.terrain:save_all(erase)
	if not erase then
		Server.object_database:update_world_decay()
		Server.object_database:clear_unused_objects()
	end
	clss:save_generator(erase)
	clss:save_markers(erase)
	Server.account_database:save_accounts(erase)
	Server.unlocks:save()
end

--- Saves the map generator state.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_generator = function(clss, erase)
	do return end -- FIXME
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM generator_settings;")
		clss.db:query("DELETE FROM generator_sectors;")
	end
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed1", tostring(Server.generator.seed1)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed2", tostring(Server.generator.seed2)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed3", tostring(Server.generator.seed3)})
	for k,v in pairs(Server.generator.sectors) do
		clss.db:query("REPLACE INTO generator_sectors (id,value) VALUES (?,?);", {k, v})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a map marker.
-- @param clss Serialize class.
-- @param marker Map marker.
Serialize.save_marker = function(clss, marker)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
		{marker.name, marker.target, marker.position.x, marker.position.y, marker.position.z,
		 marker.unlocked and 1 or 0, marker.discoverable and 1 or 0})
	clss.db:query("END TRANSACTION;")
end

--- Saves all map markers.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_markers = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM markers;")
	end
	for k,v in pairs(Marker.dict_name) do
		clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
			{k, v.target, v.position.x, v.position.y, v.position.z,
			 v.unlocked and 1 or 0, v.discoverable and 1 or 0})
	end
	clss.db:query("END TRANSACTION;")
end

------------------------------------------------------------------------------
-- Game database.

--- Initializes the game database.
-- @param self Serialize class.
-- @param reset True to clear the database.
Serialize.init_game_database = function(self, reset)
	-- Check if changes are needed.
	local version = self:get_value("game_version")
	if not reset and version == self.game_version then return end
	-- Initialize tables.
	self.db:query([[DROP TABLE IF EXISTS generator_sectors;]])
	self.db:query([[DROP TABLE IF EXISTS generator_settings;]])
	self.db:query([[DROP TABLE IF EXISTS markers;]])
	self.db:query([[DROP TABLE IF EXISTS options;]])
	self.db:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE generator_sectors (
		id INTEGER PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE generator_settings (
		key TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE markers (
		name TEXT PRIMARY KEY,
		id INTEGER,
		x FLOAT,
		y FLOAT,
		z FLOAT,
		unlocked INTENGER,
		discoverable INTEGER);]])
end

--- Gets a value from the key-value database.
-- @param self Serialize class.
-- @param key Key string.
-- @return Value string or nil.
Serialize.get_value = function(self, key)
	-- Check that the options table exists.
	-- This is just to silence an error message.
	local rows1 = self.db:query([[SELECT name FROM sqlite_master WHERE type='table' AND name='options';]])
	if not rows1 then return end
	if #rows1 == 0 then return end
	-- Get the value from the options table
	local rows = self.db:query([[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Stores a value to the key-value database.
-- @param self Serialize class.
-- @param key Key string.
-- @param value Value string.
Serialize.set_value = function(self, key, value)
	self.db:query([[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

return Serialize


