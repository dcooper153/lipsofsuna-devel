Gui = Class()

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	-- Inventory menu.
	Gui.inventory_group = Group{cols = 1, style = "inventory"}
	Gui.inventory_group:set_request{width = 100}
	Gui.inventory_group:append_row(Button{style = "inventory-label", text = "Inventory"})
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
		{"Skills", Skills.window}}
	-- Admin menu.
	Gui.menu_widget_admin = Widgets.Menu{
		{"Editor", function() Editing:toggle() end},
		{"Save", function() Network:send{packet = Packet(packets.ADMIN_SAVE)} end},
		{"Shutdown", function() Network:send{packet = Packet(packets.ADMIN_SHUTDOWN)} end}}
	Gui.menu_widget_main = Widgets.Menu{
		{"Action", Gui.menu_widget_actions},
		{"View", Gui.menu_widget_view},
		{"Admin", Gui.menu_widget_admin},
		{"Options", Options.group},
		{"Help", Help.menu}}
	Gui.menus = Widgets.Menus()
	Gui.menus:open{level = 1, widget = Gui.menu_widget_main}
	-- Bottom HUD.
	Gui.action_label = Button{style = "label", visible = false}
	Gui.chat_history = Widgets.Log()
	Gui.chat_entry = Entry{style = "chat"}
	Gui.chat_entry.activated = function(self)
		Network:send{packet = Packet(packets.CHAT, "string", self.text)}
		self:clear()
	end
	Gui.chat_group = Group{cols = 1}
	Gui.chat_group:append_row(Gui.chat_history)
	Gui.chat_group:append_row(Gui.chat_entry)
	Gui.chat_group:append_row(Gui.action_label)
	Gui.chat_group:set_expand{row = 1}
	-- Skill group.
	Gui.skill_health = Scroll{style = "health"}
	Gui.skill_health:set_range{min = 0, max = 100}
	Gui.skill_health:set_request{width = 100}
	Gui.skill_mana = Scroll{style = "mana"}
	Gui.skill_mana:set_range{min = 0, max = 100}
	Gui.skill_mana:set_request{width = 100}
	local skillgrp = Group{cols = 1, spacings = {0,1}}
	skillgrp:append_row(Gui.skill_health)
	skillgrp:append_row(Gui.skill_mana)
	-- Chat group.
	Gui.skills_group = Group{rows = 1}
	Gui.skills_group:append_col(skillgrp)
	Gui.skills_group:append_col(Quickslots.group)
	Gui.skills_group:set_expand{col = 2}
	-- Packing.
	Gui.fps_label = Button{style = "label"}
	Gui.editor_group = Group{cols = 1, style = "window", visible = false}
	Gui.editor_group:append_row(Button{style = "inventory-label", text = "Editor"})
	Gui.editor_group:append_row(Editing.dialog)
	Gui.menu_group = Group{rows = 1}
	Gui.menu_group:append_col(Gui.menus)
	Gui.menu_group:append_col(Gui.chat_group)
	Gui.menu_group:set_expand{col = 2}
	Gui.label_target = Button{style = "label"}
	Gui.center_group = Group{cols = 1}
	Gui.center_group:append_row(Gui.label_target)
	Gui.center_group:append_row(Gui.fps_label)
	Gui.center_group:append_row(Gui.menu_group)
	Gui.center_group:append_row(Gui.skills_group)
	Gui.center_group:set_expand{row = 2}
	Gui.scene = Group{rows = 1, style = "scene", margins = {5,5,5,5}}
	Gui.scene.scene = Scene()
	Gui.scene.pick = function(self) return Target:pick_scene() end
	Gui.scene.render = function(self)
		Player.camera.viewport = {self.x, self.y, self.width, self.height}
		self.scene:draw_begin{
			modelview = Player.camera.modelview,
			projection = Player.camera.projection,
			viewport = Player.camera.viewport}
		self.scene:draw_deferred_begin()
		self.scene:draw_deferred_opaque()
		self.scene:draw_deferred_end()
		self.scene:draw_forward_transparent()
		self.scene:draw_post_process{shader = "postprocess-vert-hdr"}
		self.scene:draw_post_process{shader = "postprocess-horz-hdr"}
		self.scene:draw_end()
		Speech:draw{
			modelview = Player.camera.modelview,
			projection = Player.camera.projection,
			viewport = Player.camera.viewport}
	end
	Gui.scene:append_col(Gui.editor_group)
	Gui.scene:append_col(Gui.center_group)
	Gui.scene:set_expand{col = 2, row = 1}
	Gui.bottom = Group{rows = 1}
	Gui.bottom:append_col(Gui.scene)
	Gui.bottom:append_col(nil)
	Gui.bottom:set_expand{col = 1, row = 1}
	Gui.main = Group{cols = 1, behind = true, fullscreen = true}
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
	if text then
		clss.action_label.text = text
		clss.action_label.visible = true
	else
		clss.action_label.visible = false
	end
end

Eventhandler{type = "tick", func = function(self, args)
	if Gui.fps_label then
		Gui.fps_label.text = "FPS: " .. math.floor(Client.fps + 0.5)
	end
end}
