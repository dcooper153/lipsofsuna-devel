require "system/file"

Unittest = {}
Unittest.tests = {}

--- Adds a unittest.
-- @param self Unittest class.
-- @param stage Stage number.
-- @param name Test name.
-- @param func Test function.
Unittest.add = function(self, stage, name, func)
	assert(not self.tests[name])
	self.tests[name] = {stage, name, func}
end

--- Adds all unittests in the tests directory.
-- @param self Unittest class.
Unittest.add_all = function(self)
	File:require_directory("tests")
	File:require_directory("tests/lipsofsuna")
end

--- Runs all loaded unittests.
-- @param self Unittest class.
Unittest.run_all = function(self)
	-- Sort the tests by stage and name.
	local sorted = {}
	for k,v in pairs(self.tests) do
		table.insert(sorted, v)
	end
	table.sort(sorted, function(a,b)
		if a[1] < b[1] then return true end
		if a[1] > b[1] then return false end
		if a[2] < b[2] then return true end
		if a[2] > b[2] then return false end
		return true
	end)
	-- Run the tests.
	for k,v in ipairs(sorted) do
		print("Testing " .. v[2] .. "...")
		local s,e = pcall(v[3])
		if not s then print(e) end
	end
end

Unittest:add_all()
Unittest:run_all()
