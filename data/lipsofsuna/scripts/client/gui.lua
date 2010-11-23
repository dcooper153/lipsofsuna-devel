Gui = Class()

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	-- Inventory menu.
	Gui.inventory_group = Widget{cols = 1}
	Gui.inventory_group:set_request{width = 100}
	Gui.inventory_group:append_row(Widgets.Label{font = "medium", text = "Inventory"})
	Gui.inventory_group:append_row(Equipment.group)
	Gui.inventory_group:append_row(Inventory.group)
	Gui.inventory_group:set_expand{col = 1, row = 3}
	-- Action menu.
	Gui.menu_widget_actions = Widgets.Menu{
		{"Examine", function() Commands:examine() end},
		{"Pick up", function() Commands:pickup() end},
		{"Throw", function() Commands:throw() end},
		{"Use", function() Commands:use() end}}
	-- View menu.
	Gui.menu_widget_view = Widgets.Menu{
		{"Inventory", Gui.inventory_group},
		{"Quests", Quests.window},
		{"Skills", Skills.window}}
	-- Admin menu.
	Gui.menu_widget_admin = Widgets.Menu{
		{"Editor", Editing.dialog},
		{"Save", function() Network:send{packet = Packet(packets.ADMIN_SAVE)} end},
		{"Shutdown", function() Network:send{packet = Packet(packets.ADMIN_SHUTDOWN)} end}}
	Gui.menu_widget_main = Widgets.Menu{
		{"Action", Gui.menu_widget_actions},
		{"View", Gui.menu_widget_view},
		{"Admin", Gui.menu_widget_admin},
		{"Options", Options.group},
		{"Help", Help.menu},
		{"Quit", function() Program.quit = true end}}
	Gui.menus = Widgets.Menus()
	Gui.menus:open{level = 1, widget = Gui.menu_widget_main}
	-- Bottom HUD.
	Gui.chat_history = Widgets.Log()
	Gui.chat_entry = Widgets.Entry{transparent = true}
	Gui.chat_entry.pressed = function(self)
		Network:send{packet = Packet(packets.CHAT, "string", self.text)}
		self:clear()
	end
	Gui.chat_group = Widget{cols = 1}
	Gui.chat_group:append_row(Gui.chat_history)
	Gui.chat_group:append_row(Gui.chat_entry)
	Gui.chat_group:set_expand{col = 1, row = 1}
	-- Skill group.
	Gui.skill_health = Widgets.SkillControl{compact = true}
	Gui.skill_mana = Widgets.SkillControl{compact = true}
	local skillgrp = Widget{cols = 1, spacings = {0,1}}
	skillgrp:append_row(Gui.skill_health)
	skillgrp:append_row(Gui.skill_mana)
	-- Chat group.
	Gui.skills_group = Widget{rows = 1}
	Gui.skills_group:append_col(skillgrp)
	Gui.skills_group:append_col(Quickslots.group)
	Gui.skills_group:set_expand{col = 2}
	-- Packing.
	Gui.fps_label = Widgets.Label{valign = 0}
	Gui.top_group = Widget{rows = 1}
	Gui.top_group:append_col(Gui.chat_group)
	Gui.top_group:append_col(Gui.fps_label)
	Gui.top_group:set_expand{col = 1}
	Gui.center_group = Widget{cols = 1}
	Gui.center_group:append_row(Gui.top_group)
	Gui.center_group:append_row(Gui.menus)
	Gui.center_group:append_row(Gui.skills_group)
	Gui.center_group:set_expand{col = 1, row = 1}
	Gui.scene = Widgets.Scene{rows = 1, camera = Player.camera, margins = {5,5,5,5}}
	Gui.scene:append_col(Gui.center_group)
	Gui.scene:set_expand{col = 1, row = 1}
	Gui.bottom = Widget{rows = 1}
	Gui.bottom:append_col(Gui.scene)
	Gui.bottom:append_col(nil)
	Gui.bottom:set_expand{col = 1, row = 1}
	Gui.main = Widget{cols = 1, behind = true, fullscreen = true}
	Gui.main:append_row(Gui.bottom)
	Gui.main:set_expand{col = 1, row = 1}
	Gui.main.floating = true
end

--- Frees the in-game user interface.
-- @param clss Gui class.
Gui.free = function()
	Gui.main.floating = false
end

--- Sets or unsets the text of the action label.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_action_text = function(clss, text)
	clss.scene.action = text
end

--- Sets or unsets the active target.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_target_text = function(clss, text)
	clss.scene.action = text
end

Eventhandler{type = "tick", func = function(self, args)
	if Gui.fps_label then
		Gui.fps_label.text = "FPS: " .. math.floor(Client.fps + 0.5)
	end
end}
