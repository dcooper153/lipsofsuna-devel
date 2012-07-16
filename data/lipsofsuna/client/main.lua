print "INFO: Loading client scripts."

Render.skybox = "skybox1"
Reload.enabled = true

require "client/client"
File:require_directory("client/widgets")
require "client/action"
require "client/audio"
require "client/event"
require "client/theme"
require "client/controls"
require "client/effect"
require "client/quickslots"
require "client/lighting"
Lighting:init()
require "client/player"
require "client/simulation"
require "client/target"
File:require_directory("client/objects")

-- Initialize the UI state.
Ui:init()
Client:init()
Client.options:apply()
if Settings.join then
	Client:join_game()
elseif Settings.host then
	Client:host_game()
elseif Settings.editor then
	Ui.state = "editor"
elseif Settings.benchmark then
	Ui.state = "benchmark"
	Client.benchmark = Benchmark()
else
	Ui.state = "mainmenu"
end

Program.profiling = {}

-- Main loop.
local frame = Program.time
while not Program.quit do
	-- Update program state.
	if Settings.watchdog then
		Program:watchdog_start(30)
	end
	local t1 = Program.time
	Program:update()
	Program:update_scene(t1 - frame)
	local t2 = Program.time
	Eventhandler:update()
	Client:update(t1 - frame)
	local t3 = Program.time
	-- Render the scene.
	Program:render_scene()
	local t4 = Program.time
	-- Update profiling stats.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
	Program.profiling.render = t4 - t3
	frame = t1
end

-- Make sure that the game was saved.
Client:terminate_game()
