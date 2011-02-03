print "INFO: Loading client scripts."

local db = Database{name = "client.sql"}
Sectors.instance = Sectors{database = db, save_objects = false}
Sectors.instance:erase_world()

Views = {}

require "client/widgets/background"
require "client/widgets/button"
require "client/widgets/check"
require "client/widgets/colorselector"
require "client/widgets/combobox"
require "client/widgets/cursor"
require "client/widgets/dialogchoice"
require "client/widgets/dialoglabel"
require "client/widgets/entry"
require "client/widgets/equipment"
require "client/widgets/icon"
require "client/widgets/iconbutton"
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
require "client/widgets/popup"
require "client/widgets/progress"
require "client/widgets/questinfo"
require "client/widgets/scene"
require "client/widgets/skillcontrol"
require "client/action"
require "client/audio"
require "client/theme"
require "client/client"
require "client/drag"
require "client/commands"
require "client/controls"
require "client/container"
require "client/effect"
require "client/inventory"
require "client/equipment"
require "client/quickslots"
require "client/gui"
require "client/object"
require "client/slots"
require "client/target"
require "client/shaders/adamantium"
require "client/shaders/default"
require "client/shaders/deferred"
require "client/shaders/eye"
require "client/shaders/glass"
require "client/shaders/foliage"
require "client/shaders/hair"
require "client/shaders/luminous"
require "client/shaders/normalmap"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/shadowmap"
require "client/shaders/skin"
require "client/shaders/terrain"
require "client/shaders/widget"
require "client/views/book"
require "client/views/chargen"
require "client/views/crafting"
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

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

local animt = 0
local ipolt = 0
Eventhandler{type = "tick", func = function(self, args)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
	-- Update animations.
	animt = animt + args.secs
	if animt > 0.2 * (1 - Views.Options.inst.animation_quality) then
		for k,v in pairs(Object.objects) do
			if v.animated then
				v:update_animations{secs = animt}
				v:deform_mesh()
			end
		end
		animt = 0
	end
	-- Interpolate objects.
	ipolt = math.min(ipolt + args.secs, 1)
	while ipolt > 1/60 do
		for k,v in pairs(Object.objects) do
			v:update_motion_state(1/60)
		end
		ipolt = ipolt - 1/60
	end
	-- Update equipment positions.
	for k,v in pairs(Slots.dict_owner) do
		v:update()
	end
	-- Update player state.
	if Player.object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Player:update_camera(args.secs)
		-- Update the light ball.
		Player.light.position = Player.object.position + Player.object.rotation * Vector(0, 2, -3)
		Player.light.enabled = true
		-- Sound playback.
		Sound.listener_position = Player.object.position
		Sound.listener_rotation = Player.object.rotation
		Sound.listener_velocity = Player.object.velocity
		-- Refresh the active portion of the map.
		Player.object:refresh()
	end
end}

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
