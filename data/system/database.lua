local Class = require("system/class")
local Packet = require("system/packet")

if not Los.program_load_extension("database") then
	error("loading extension `database' failed")
end

------------------------------------------------------------------------------

local Database = Class("Database")

--- Opens a database.
-- @param clss Database class.
-- @param name Database name.
-- @return New database.
Database.new = function(clss, name)
	local self = Class.new(clss)
	self.handle = Los.database_new(name)
	if not self.handle then
		assert(self.handle, string.format("creating database %q failed", name))
	end
	return self
end

--- Queries the database.<br/>
-- Executes an SQLite query and returns the results in a table. The returned
-- table contains a list of tables that denote the rows of the result. The row
-- tables further contain a list of values that denote the contents of the columns.<br/>
-- You can avoid escaping the arguments required by the query by writing a `?' in
-- place of the argument in the query and then passing the value in the binding
-- array. The binding array is a simple table that contains the arguments in the
-- same order as the query.
-- @param self Database.
-- @param args Arguments.<ul>
--   <li>1: Query string.</li>
--   <li>2: Array of values to bind to the statement.</ul>
-- @return Table of rows.
Database.query = function(self, a, b)
	-- Translate packets to handles.
	local s
	if b then
		s = {}
		for k,v in pairs(b) do
			s[k] = (type(v) == "table") and v.handle or v
		end
	end
	-- Perform the query.
	local t = Los.database_query(self.handle, a, s)
	if not t then return end
	-- Translate handles to packets.
	for k1,v1 in pairs(t) do
		for k2,v2 in pairs(v1) do
			if type(v2) == "userdata" then v1[k2] = Packet:new_from_handle(v2) end
		end
	end
	return t
end

--- Gets the approximate memory used by all databases.
-- @param self Database class.
-- @return Size in bytes.
Database.get_memory_used = function(self)
	return Los.database_get_memory_used()
end

return Database
