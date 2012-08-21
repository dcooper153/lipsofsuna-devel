Unittest:add(1, "system", "network", function()
	local Network = require("system/network")
	-- Host.
	assert(not Network:get_connected())
	assert(Network:host{port = 101010})
	assert(Network:get_connected())
	-- Close.
	assert(not Network:get_closed())
	Network:set_closed(true)
	assert(Network:get_closed())
	Network:set_closed(false)
	assert(not Network:get_closed())
	-- Shutdown.
	assert(Network:get_connected())
	Network:shutdown()
	assert(not Network:get_connected())
end)
