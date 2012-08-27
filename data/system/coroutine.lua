--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.coroutine
-- @alias Coroutine

local Class = require("system/class")
local Eventhandler = require("system/eventhandler")

--- TODO:doc
-- @type Coroutine
local Coroutine = Class("Coroutine")
Coroutine.routines = {}

--- Creates a thread executing a function.
-- @param clss Coroutine class.
-- @param func Function to execute.
-- @return Coroutine.
Coroutine.new = function(clss, func)
	local co = coroutine.create(func)
	if coroutine.status(co) == "dead" then return end
	clss.routines[co] = co
	return co
end

Coroutine.sleep = function(self, delay)
	local t = 0.0
	while t < delay do t = t + coroutine.yield() end
end

--- Updates all the threads.
-- @param clss Coroutine class.
-- @param secs Number of seconds since the last update.
Coroutine.update = function(clss, secs)
	for key,value in pairs(clss.routines) do
		local ret,err = coroutine.resume(value, secs)
		if not ret then
			print(debug.traceback(value, "ERROR: " .. err))
		end
		if coroutine.status(value) == "dead" then
			clss.routines[key] = nil
		end
	end
end

Coroutine.yield = function(self)
	return coroutine.yield()
end

-- Updating threads.
Eventhandler{type = "tick", func = function(self, args)
	Coroutine:update(args.secs)
end}

return Coroutine


