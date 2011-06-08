print "INFO: Loading client scripts."

Client.db = Database{name = "client.sqlite"}
Client.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
Sectors.instance = Sectors{database = Client.db, save_objects = false}
Sectors.instance:erase_world()

Views = {}
Physics.GROUP_OBJECT = 0x0001
Physics.GROUP_EQUIPMENT = 0x0002
Physics.GROUP_CROSSHAIR = 0x0004
Physics.GROUP_PLAYER = 0x0008
Physics.GROUP_STATIC = 0x4000
Physics.GROUP_TILES = 0x8000
Physics.MASK_CAMERA = 0xF001
Physics.MASK_PICK = 0xF001

require "client/widgets/background"
require "client/widgets/colorselector"
require "client/widgets/container"
require "client/widgets/dialogchoice"
require "client/widgets/dialoglabel"
require "client/widgets/equipment"
require "client/widgets/feattooltip"
require "client/widgets/icon"
require "client/widgets/itembutton"
require "client/widgets/itemlist"
require "client/widgets/itemtooltip"
require "client/widgets/log"
require "client/widgets/menu"
require "client/widgets/menus"
require "client/widgets/modifier"
require "client/widgets/modifiers"
require "client/widgets/progress"
require "client/widgets/questinfo"
require "client/widgets/quickslot"
require "client/widgets/scene"
require "client/widgets/skills"
require "client/widgets/skillcontrol"
require "client/widgets/skilltooltip"
require "client/widgets/text"
require "client/widgets/tooltip"
require "client/action"
require "client/audio"
require "client/event"
require "client/theme"
require "client/client"
require "client/drag"
require "client/commands"
require "client/controls"
require "client/effect"
require "client/equipment"
require "client/quickslots"
require "client/gui"
require "client/object"
require "client/player"
require "client/slots"
require "client/target"
require "client/network/object"
require "client/network/trading"
for k,v in pairs(File:scan_directory("scripts/client/shaders")) do
	require("client/shaders/" .. string.gsub(v, "([^.]*).*", "%1"))
end
require "client/views/admin"
require "client/views/book"
require "client/views/chargen"
require "client/views/controls"
require "client/views/dialog"
require "client/views/editing"
require "client/views/feats"
require "client/views/game"
require "client/views/help"
require "client/views/inventory"
require "client/views/options"
require "client/views/quests"
require "client/views/skills"
require "client/views/startup"

Player.crosshair = Object{model = "crosshair1", collision_group = Physics.GROUP_CROSSHAIR}

-- Initialize the UI state.
Widgets.Cursor.inst = Widgets.Cursor(Iconspec:find{name = "cursor1"})
Gui:init()
Gui:set_mode("startup")

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
	Widgets:update()
end
