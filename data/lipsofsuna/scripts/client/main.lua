print "INFO: Loading client scripts."

local db = Database{name = "client.sql"}
Sectors.instance = Sectors{database = db, save_objects = false}
Sectors.instance:erase_world()

Views = {}

require "client/widgets/widgets"
require "client/widgets/background"
require "client/widgets/button"
require "client/widgets/colorselector"
require "client/widgets/combobox"
require "client/widgets/container"
require "client/widgets/cursor"
require "client/widgets/dialogchoice"
require "client/widgets/dialoglabel"
require "client/widgets/entry"
require "client/widgets/equipment"
require "client/widgets/frame"
require "client/widgets/icon"
require "client/widgets/itembutton"
require "client/widgets/itemlist"
require "client/widgets/label"
require "client/widgets/listwidget"
require "client/widgets/log"
require "client/widgets/menu"
require "client/widgets/menuitem"
require "client/widgets/menus"
require "client/widgets/modifier"
require "client/widgets/modifiers"
require "client/widgets/progress"
require "client/widgets/questinfo"
require "client/widgets/quickslot"
require "client/widgets/scene"
require "client/widgets/scrollbar"
require "client/widgets/skills"
require "client/widgets/skillcontrol"
require "client/widgets/text"
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
require "client/shaders/adamantium"
require "client/shaders/default"
require "client/shaders/diffnormspec"
require "client/shaders/eye"
require "client/shaders/glass"
require "client/shaders/foliage"
require "client/shaders/fur"
require "client/shaders/hair"
require "client/shaders/luminous"
require "client/shaders/normalmap"
require "client/shaders/nolitdiff"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/skin"
require "client/shaders/terrain"
require "client/shaders/widget"
require "client/views/book"
require "client/views/chargen"
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

Player.crosshair = Object{model = "crosshair1"}

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
