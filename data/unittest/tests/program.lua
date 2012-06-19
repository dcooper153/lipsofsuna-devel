Unittest:add(1, "program", function()
	require "system/core"
	-- Built-in tests.
	Los.program_unittest()
	-- Getters and setters.
	assert(type(Program.args) == "string")
	assert(type(Program.quit) == "boolean")
	assert(type(Program.sectors) == "table")
	assert(type(Program.sleep) == "number")
	assert(type(Program.tick) == "number")
	assert(type(Program.time) == "number")
	Program.quit = true
	assert(Program.quit)
	-- Message passing disabled for non-threads.
	assert(not Program:push_message("fail", "fail"))
	assert(not Program:pop_message())
end)
