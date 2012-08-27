--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.thread
-- @alias Thread

local Class = require("system/class")
local Model = require("system/model")

if not Los.program_load_extension("thread") then
	error("loading extension `thread' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Thread
local Thread = Class("Thread")

--- Creates a new thread.
-- @param clss Thread class.
-- @param ... Arguments.<ul>
--   <li>1,file: Script path to execute.</li>
--   <li>2,args: Argument string to pass./<li></ul>
--   <li>3,code: Code string to execute./<li></ul>
-- @return Thread.
Thread.new = function(clss, ...)
	local self = Class.new(clss)
	self.handle = Los.thread_new(...)
	assert(self.handle, "thread creation failed")
	return self
end

--- Pops a message sent by the child script of the thread.
-- @param self Thread.
-- @return Message table or nil.
Thread.pop_message = function(self)
	local r = Los.thread_pop_message(self.handle)
	if not r then return end
	if r.type == "model" and r.model then
		r.model = Model:new_from_handle(r.model)
	end
	return r
end

--- Pushes a message to the child script of the thread.
-- @param self Thread.
-- @param ... Message arguments.
-- @return True on success.
Thread.push_message = function(self, ...)
	local a = ...
	if type(a) == "table" then
		if a.model then a.model = a.model.handle end
		return Los.thread_push_message(self.handle, a)
	else
		return Los.thread_push_message(self.handle, ...)
	end
end

Thread.get_done = function(self)
	return Los.thread_get_done(self.handle)
end

Thread.set_quit = function(self, v)
	return Los.thread_set_quit(self.handle, v)
end

return Thread


