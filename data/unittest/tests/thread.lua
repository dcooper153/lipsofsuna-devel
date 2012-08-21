Unittest:add(1, "system", "thread", function()
	local Model = require("system/model")
	local Thread = require("system/thread")
	-- Creation.
	print("Testing thread creation...")
	local t = Thread(nil, "Hello World!", [[
		assert(Los.program_get_args() == "Hello World!")]])
	assert(t)
	-- Message passing.
	print("Testing thread message passing...")
	local t1 = Thread(nil, "", [[
		local Program = require("system/core")
		local m
		repeat m = Program:pop_message() until m
		assert(m.type == "string")
		assert(m.name == "name1")
		assert(m.string == "string1")
		assert(Program:push_message("name2", "string2"))]])
	assert(t1:push_message("name1", "string1"))
	local m
	repeat m = t1:pop_message() until m
	assert(m.type == "string")
	assert(m.name == "name2")
	assert(m.string == "string2")
	-- Model passing.
	print("Testing thread message model passing...")
	local t2 = Thread(nil, "", [[
		local Program = require("system/core")
		local Model = require("system/model")
		assert(Program:push_message{model = Model()})]])
	repeat m = t2:pop_message() until m
	assert(m.type == "model")
	assert(m.model)
	assert(m.model.class_name == "Model")
	-- Thread termination.
	print("Testing thread termination...")
	local t3 = Thread(nil, "", [[
		local Program = require("system/core")
		while not Program:get_quit() do Program:update() end]])
	assert(not t3:get_done())
	t3:set_quit(true)
	repeat until t3:get_done()
end)
