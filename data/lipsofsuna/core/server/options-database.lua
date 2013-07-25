--- Options database.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.options_database
-- @alias OptionsDatabase

local Class = require("system/class")

--- Options database.
-- @type OptionsDatabase
local OptionsDatabase = Class("OptionsDatabase")

--- Creates a new options database.
-- @param clss OptionsDatabase class.
-- @param db Database.
-- @return OptionsDatabase.
OptionsDatabase.new = function(clss, db)
	local self = Class.new(clss)
	self.__db = db
	self.__version = "8"
	return self
end

--- Checks if the version of the save file is supported.
-- @param self OptionsDatabase.
-- @return True if the version is supported. False otherwise.
OptionsDatabase.check_version = function(self)
	local version = self:get_value("game_version")
	if not version then return true end
	if version == self.__version then return true end
end

--- Saves the database.
-- @param self OptionsDatabase.
-- @param reset True to clear the database. False otherwise.
OptionsDatabase.save = function(self, reset)
	if reset then
		self.__db:query([[DROP TABLE IF EXISTS options;]])
		self.__db:query(
			[[CREATE TABLE options (
			key TEXT PRIMARY KEY,
			value TEXT);]])
	end
	self:set_value("game_version", self.__version)
end

--- Gets a value from the key-value database.
-- @param self OptionsDatabase.
-- @param key Key string.
-- @return Value string if found. Nil otherwise.
OptionsDatabase.get_value = function(self, key)
	-- Check that the options table exists.
	-- This is just to silence an error message.
	local rows1 = self.__db:query([[SELECT name FROM sqlite_master WHERE type='table' AND name='options';]])
	if not rows1 then return end
	if #rows1 == 0 then return end
	-- Get the value from the options table
	local rows = self.__db:query([[SELECT value FROM options WHERE key=?;]], {key})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Stores the key-value pair into the database.
-- @param self OptionsDatabase.
-- @param key Key string.
-- @param value Value string.
OptionsDatabase.set_value = function(self, key, value)
	self.__db:query([[REPLACE INTO options (key,value) VALUES (?,?);]], {key, value})
end

return OptionsDatabase
