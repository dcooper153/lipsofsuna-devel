require "system/model"

if not Los.program_load_extension("thread") then
	error("loading extension `thread' failed")
end

------------------------------------------------------------------------------

Thread = Class()

--- Creates a new thread.
-- @param clss Thread class.
-- @param ... Arguments.<ul>
--   <li>1,file: Script path to execute.</li>
--   <li>2,args: Argument string to pass./<li></ul>
--   <li>3,code: Code string to execute./<li></ul>
-- @return Thread.
Thread.new = function(clss, ...)
	local h = Los.thread_new(...)
	assert(h, "thread creation failed")
	return Class.new(Thread, {handle = h})
end

--- Pops a message sent by the child script of the thread.
-- @param self Thread.
-- @return Message table or nil.
Thread.pop_message = function(self)
	local r = Los.thread_pop_message(self.handle)
	if not r then return end
	if r.type == "model" and r.model then
		r.model = Class.new(Model, {handle = r.model})
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

Thread:add_getters{
	done = function(self) return Los.thread_get_done(self.handle) end}

Thread:add_setters{
	done = function(self, v) end,
	quit = function(self, v) return Los.thread_set_quit(self.handle, v) end}
