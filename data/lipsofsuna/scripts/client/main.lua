print "INFO: Loading client scripts."

local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		Program:load_extension(string.sub(arg, e + 1))
	else
		oldrequire(arg)
	end
end

require "core/camera"
require "core/network"
require "core/object-render"
require "core/render"
require "core/sound"
require "core/speech"
require "core/tiles"
require "core/tiles-physics"
require "core/tiles-render"
require "core/reload"
require "core/widgets"
Voxel.blocks_per_line = 4
Voxel.tiles_per_line = 8

require "common/effect"
require "common/eventhandler"
require "common/faction"
require "common/feat"
require "common/itemspec"
require "common/material"
require "common/model"
require "common/object"
require "common/obstaclespec"
require "common/pattern"
require "common/protocol"
require "common/quest"
require "common/species"
require "common/thread"
require "common/timer"
require "content/effects"
require "content/factions"
require "content/feats"
require "content/items"
require "content/materials"
require "content/obstacles"
require "content/patterns"
require "content/quests"
require "content/species"
require "client/widgets/background"
require "client/widgets/button"
require "client/widgets/combobox"
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
require "client/widgets/popup"
require "client/widgets/progress"
require "client/widgets/questinfo"
require "client/widgets/skillcontrol"
require "client/action"
require "client/theme"
require "client/client"
require "client/crafting"
require "client/book"
require "client/commands"
require "client/controls"
require "client/container"
require "client/dialog"
require "client/effect"
require "client/inventory"
require "client/equipment"
require "client/quickslots"
require "client/help"
require "client/gui"
require "client/object"
require "client/options"
require "client/shader"
require "client/chargen"
require "client/startup"
require "client/skills"
require "client/slots"
require "client/target"
require "client/quest"
require "client/editing"
require "client/shaders/default"
require "client/shaders/deferred"
require "client/shaders/glass"
require "client/shaders/hair"
require "client/shaders/normalmap"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/shadeless"
require "client/shaders/shadowmap"
require "client/shaders/skin"
require "client/shaders/texrefl"
require "client/shaders/tilenfn"
require "client/shaders/tilenxz"
require "client/shaders/widget"

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

local animt = 0
Eventhandler{type = "tick", func = function(self, args)
	animt = animt + args.secs
	if animt > 0.03 then
		for k,v in pairs(Object.objects) do
			if v.type == "creature" then
				v:update_animations{secs = animt}
				v:deform_mesh()
			end
		end
		animt = 0
	end
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
