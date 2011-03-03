if Program.args == "--help" or Program.args == "-h" then
	print("Usage: lipsofsuna [modname] [options]\n")
	print("Options:")
	print("  --help                          Show this help message and exit.\n")
	print("Examples:")
	print("  lipsofsuna lipsofsuna --help    Show help for mod lipsofsuna")
	print("  lipsofsuna lipsofsuna --server  Run mod lipsofsuna as a dedicated server")
	return
end

require "system/database"
require "system/eventhandler"
require "system/graphics"
require "system/render"
require "system/sound"
require "system/widgets"
require "launcher"
require "theme"
require "widget"
require "startup"

Widgets.Cursor.inst = Widgets.Cursor({
	name = "cursor1",
	image = "cursor1",
	offset = {0, 0},
	size = {32, 32}})

Eventhandler{type = "mousepress", func = function(self, args)
	Widgets:handle_event(args)
end}

Eventhandler{type = "keypress", func = function(self, args)
	Widgets:handle_event(args)
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

-- Main loop.
while not Program.quit do
	-- Update program state.
	Program:update()
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	-- Update the cursor.
	Widgets.Cursor.inst:update()
	-- Render the scene.
	Client:clear_buffer()
	Widgets:draw()
	Client:swap_buffers()
	-- Focus widgets.
	Widgets:update()
end
