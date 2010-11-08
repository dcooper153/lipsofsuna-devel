require "core/graphics"
require "core/render"
require "core/sound"
require "core/widgets"
require "common/eventhandler"
require "client/entry"
require "client/label"
require "client/theme"
require "client/widget"
require "client/startup"

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
