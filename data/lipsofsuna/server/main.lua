Game:init("server", Settings.file, Settings.port)
Server:load()

Program.sleep = 1/60
Program.profiling = {}

-- Main loop.
while not Program.quit do
	-- Update program state.
	local t1 = Program.time
	Program:update()
	local t2 = Program.time
	Eventhandler:update()
	local t3 = Program.time
	-- Store timings.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
end

-- Save at exit.
Server:deinit()
