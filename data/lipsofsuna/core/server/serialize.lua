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
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	if erase then Server.object_database:clear_objects() end
	Main.game.sectors:save_world(erase)
	if not erase then
		Server.object_database:update_world_decay()
		Server.object_database:clear_unused_objects()
	end
	Server.account_database:save_accounts(erase)
	Main.unlocks:save()
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
	self.db:query([[DROP TABLE IF EXISTS options;]])
	self.db:query(
		[[CREATE TABLE options (
		key TEXT PRIMARY KEY,
		value TEXT);]])
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


