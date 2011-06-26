require "client/action"
require "client/camera"
require "client/shader"
require "client/target"
require "client/theme"
require "client/shaders/adamantium"
require "client/shaders/default"
require "client/shaders/diffnormspec"
require "client/shaders/eye"
require "client/shaders/glass"
require "client/shaders/foliage"
require "client/shaders/fur"
require "client/shaders/hair"
require "client/shaders/lava"
require "client/shaders/luminous"
require "client/shaders/normalmap"
require "client/shaders/nolitdiff"
require "client/shaders/partblend"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/skin"
require "client/shaders/speedline"
require "client/shaders/terrain"
require "client/shaders/water"
require "client/shaders/widget"
require "client/widgets/background"
require "client/widgets/scene"
require "editor/controls"
require "editor/events"
require "editor/selection"
require "editor/editorobject"

for k,s in pairs(Shader.dict_name) do
	s:set_quality(2)
end

require "editor/editor"

------------------------------------------------------------------------------

-- Initialize the UI state.
Widgets.Cursor.inst = Widgets.Cursor(Iconspec:find{name = "cursor1"})
Editor.inst = Editor()
if not Editor.inst then
	Program.quit = true
end

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
	Program:clear_buffer()
	Widgets:draw()
	Program:swap_buffers()
	-- Focus widgets.
	Widgets:update()
end
