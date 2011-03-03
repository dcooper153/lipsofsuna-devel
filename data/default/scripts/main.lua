if Program.args == "--help" or Program.args == "-h" then
	print("Usage: lipsofsuna [modname] [options]\n")
	print("Options:")
	print("  --help                          Show this help message and exit.\n")
	print("Examples:")
	print("  lipsofsuna lipsofsuna --help    Show help for mod lipsofsuna")
	print("  lipsofsuna lipsofsuna --server  Run mod lipsofsuna as a dedicated server")
	return
end

require "system/graphics"
require "system/render"
require "system/sound"
require "system/widgets"
require "eventhandler"
require "cursor"
require "entry"
require "label"
require "theme"
require "widget"
require "startup"

Widgets.Cursor.inst = Widgets.Cursor({
	name = "cursor1",
	image = "cursor1",
	offset = {0, 0},
	size = {32, 32}})

Eventhandler{type = "mousepress", func = function(self, args)
	local w = Widgets.focused_widget
	if w and w.pressed then
		w:pressed(args)
	end
end}

Eventhandler{type = "keypress", func = function(self, args)
	local w = Widgets.focused_widget
	if w and w.event then
		w:event(args)
	end
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
	local w = Widgets.focused_widget
	if Widgets.focused_widget_prev ~= w then
		if Widgets.focused_widget_prev then
			Widgets.focused_widget_prev.focused = false
		end
		if w then
			w.focused = true
		end
		Widgets.focused_widget_prev = w
	end
end
