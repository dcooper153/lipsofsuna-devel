Unittest:add(1, "database", function()
	require "system/database"
	-- Database creation.
	local d = Database("unittest.sqlite")
	assert(d)
	-- Table editing.
	d:query("DROP TABLE IF EXISTS terrain;")
	d:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data TEXT);")
	d:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {1, "unittest"})
	-- Table selection.
	local r = d:query("SELECT * FROM terrain WHERE sector=?;", {1})
	assert(type(r) == "table")
	assert(#r == 1)
	assert(type(r[1]) == "table")
	assert(r[1][1] == 1)
	assert(r[1][2] == "unittest")
end)
