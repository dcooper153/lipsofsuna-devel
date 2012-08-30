--- Operations for controlling the core features of the engine.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.core
-- @alias Program

local Class = require("system/class")
local Model = require("system/model")

--- Operations for controlling the core features of the engine.
-- @type Program
Program = Class("Program")

--- Adds a data lookup path.
-- @param clss Program class.
-- @param path Path name.
Program.add_path = function(clss, path)
	return Los.program_add_path(path)
end

--- Collects garbage for the given number of seconds.
-- @param clss Program class.
-- @param secs Seconds.
-- @return True if garbage collection finished before the end.
Program.collect_garbage = function(clss, secs)
	return Los.program_collect_garbage(secs)
end

--- Dumps debug information to the console.
-- @param self Program class.
Program.debug_dump = function(self)
	return Los.program_debug_dump()
end

--- Discards all events from the event queue.
-- @param clss Program class.
Program.discard_events = function(clss)
	repeat until not Program:pop_event()
end		

--- Gets the filename of the calling script file.
-- @param clss Program class.
-- @param index Caller index in the hierarchy.
-- @return Path relative to the data directory root.
Program.get_calling_file = function(clss, index)
	-- FIXME: Should use the module name in the regular expression.
	local absname = debug.getinfo(index).source
	local relname = string.match(absname, ".*/data/lipsofsuna/(.*)")
	return relname
end

--- Sets the name of the mod to be executed after this one quits.
-- @param clss Program class.
-- @param args Arguments.<ul>
--  <li>1,name: Module name.</li></ul>
--  <li>2,args: Argument string to pass to the module.</li></ul>
Program.launch_mod = function(clss, args)
	return Los.program_launch_mod(args)
end

--- Pops an event from the event queue.
-- @param clss Program class.
-- @return Event table, or nil.
Program.pop_event = function(clss)
	clss:pump_events()
	if not __events then return end
	local t = __events[1]
	if not t then return end
	table.remove(__events, 1)
	return t
end

--- Pops a message sent by the parent script.
-- @param self Thread.
-- @return Message table or nil.
Program.pop_message = function(self)
	local r = Los.program_pop_message()
	if not r then return end
	if r.type == "model" and r.model then
		r.model = Model:new_from_handle(r.model)
	end
	return r
end

--- Pumps engine events to the event queue.
-- @param clss Program class.
Program.pump_events = function(clss)
	Los.program_pump_events()
end

--- Pushes an event to the back of the event queue.
-- @param clss Program class.
-- @param event Event table.
Program.push_event = function(clss, event)
	if not __events then __events = {} end
	clss:pump_events()
	table.insert(__events, event)
end

--- Pushes a message to the parent script.
-- @param self Thread.
-- @param ... Message arguments.
-- @return True on success.
Program.push_message = function(self, ...)
	local a = ...
	if type(a) == "table" then
		if a.model then a.model = a.model.handle end
		return Los.program_push_message(a)
	else
		return Los.program_push_message(...)
	end
end

--- Calls the function and catches and handles errors.
-- @param clss Program class.
-- @param func Function to call.
Program.safe_call = function(clss, func)
	xpcall(func, function(err) print(debug.traceback("ERROR: " .. err)) end)
end

--- Request program shutdown.
-- @param clss Program class.
Program.shutdown = function(clss)
	Los.program_shutdown()
end

--- Updates the program state.
-- @param clss Program class.
Program.update = function(clss)
	Los.program_update()
end

--- Waits for the given number seconds.
-- @param clss Program class.
-- @param secs Delay in seconds.
Program.wait = function(clss, secs)
	Los.program_wait(secs)
end

--- Gets the argument string passed to the program at startup time.
-- @param self Program class.
-- @return String.
Program.get_args = function(self)
	return Los.program_get_args()
end

--- Gets the short term average frames per second.
-- @param self Program class.
-- @return Number.
Program.get_fps = function(self)
	return Los.program_get_fps()
end

--- Gets the quit flag of the program.
-- @param self Program class.
-- @return Boolean.
Program.get_quit = function(self)
	return Los.program_get_quit()
end

--- Sets the quit flag of the program.
-- @param self Program class.
-- @param v True.
Program.set_quit = function(self, v)
	Los.program_set_quit(v)
end

--- Gets the sleep time between ticks.
-- @param self Program class.
-- @return Timer in seconds.
Program.get_sleep = function(self)
	return Los.program_get_sleep()
end

--- Sets the sleep time between ticks.
-- @param self Program class.
-- @param v Timer in seconds.
Program.set_sleep = function(self, v)
	Los.program_set_sleep(v)
end

--- Gets the short term average tick length in seconds.
-- @param self Program class.
-- @return Number.
Program.get_tick = function(self)
	return Los.program_get_tick()
end

--- Gets the number of seconds the program has been running.
-- @param self Program class.
-- @return Number.
Program.get_time = function(self)
	return Los.program_get_time()
end

--- Gets the version string of the engine.
-- @param self Program class.
-- @return String.
Program.get_version = function(self)
	return Los.program_get_version()
end

return Program
