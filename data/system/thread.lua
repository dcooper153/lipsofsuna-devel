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

Thread.unittest = function()
	print("Creating a thread...")
	local t = Thread(nil, "Hello World!", [[
print("Thread main")
assert(Los.program_get_args() == "Hello World!")]])
	assert(t)
end
