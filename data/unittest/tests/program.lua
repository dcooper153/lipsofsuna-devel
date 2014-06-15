Unittest:add(1, "system", "program", function()
	local Program = require("system/core")
	local Time = require("system/time")
	-- Built-in tests.
	Los.program_unittest()
	-- Getters and setters.
	assert(type(Program:get_args()) == "string")
	assert(type(Program:get_quit()) == "boolean")
	assert(type(Program:get_sleep()) == "number")
	assert(type(Program:get_tick()) == "number")
	assert(type(Time:get_secs()) == "number")
	Program:set_quit(true)
	assert(Program:get_quit())
	-- Message passing disabled for non-threads.
	assert(not Program:push_message("fail", "fail"))
	assert(not Program:pop_message())
end)
