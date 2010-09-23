local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		Program:load_extension(string.sub(arg, e + 1))
	else
		oldrequire(arg)
	end
end

require "core/render"
require "core/sound"
require "core/widgets"
require "common/eventhandler"
require "client/theme"
require "client/widget"
require "client/startup"

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
	Widgets:draw()
	Client:swap_buffers()
end
