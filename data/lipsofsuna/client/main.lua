print "INFO: Loading client scripts."

Render.skybox = "skybox1"
Reload.enabled = true

require "client/client"

Physics.GROUP_OBJECT = 0x0001
Physics.GROUP_EQUIPMENT = 0x0002
Physics.GROUP_CROSSHAIR = 0x0004
Physics.GROUP_PLAYER = 0x0008
Physics.GROUP_STATIC = 0x4000
Physics.GROUP_TILES = 0x8000
Physics.MASK_CAMERA = 0xF001
Physics.MASK_PICK = 0xF001

File:require_directory("client/widgets")
require "client/action"
require "client/audio"
require "client/chat"
require "client/event"
require "client/theme"
require "client/drag"
require "client/controls"
require "client/effect"
require "client/equipment"
require "client/quickslots"
require "client/lighting"
Lighting:init()
require "client/player"
require "client/slots"
require "client/target"
File:require_directory("client/network")
File:require_directory("client/objects")

-- Initialize the UI state.
Ui:init()
Client:init()
Client.options:apply()
Player.crosshair = Object{model = "crosshair1", collision_group = Physics.GROUP_CROSSHAIR}
if Settings.join then
	Client:join_game()
elseif Settings.host then
	Client:host_game()
elseif Settings.editor then
	Ui.state = "editor"
else
	Ui.state = "mainmenu"
end

Program.profiling = {}

-- Main loop.
while not Program.quit do
	-- Update program state.
	if Settings.watchdog then
		Program:watchdog_start(30)
	end
	local t1 = Program.time
	Program:update()
	local t2 = Program.time
	Eventhandler:update()
	Client:update()
	local t3 = Program.time
	-- Render the scene.
	Widgets:draw()
	local t4 = Program.time
	-- Update profiling stats.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
	Program.profiling.render = t4 - t3
end
