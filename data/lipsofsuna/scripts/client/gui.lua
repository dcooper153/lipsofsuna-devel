Gui = Class()

Widgets.Scene = Class(Widget)

Widgets.Scene.setter = function(self, key, value)
	if key == "action" then
		if self.action ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Scene.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.scene = Scene()
	return self
end

Widgets.Scene.pick = function(self)
	return Target:pick_scene()
end

Widgets.Scene.pressed = function(self)
	Target:select_scene()
end

Widgets.Scene.render = function(self)
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

Widgets.Scene.reshaped = function(self)
	local w = self.width
	local h = self.height
	local o = 100
	self:canvas_clear()
	if self.action then
		self:canvas_text{
			dest_position = {0,h-o},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {1,1,1,1},
			text_font = "medium"}
	end
	self:canvas_compile()
end

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
	Gui.chat_history = Widgets.Log()
	Gui.chat_entry = Widgets.Entry()
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
	Gui.editor_group = Widgets.Popup{cols = 1, visible = false}
	Gui.editor_group:append_row(Widgets.Label{font = "medium", text = "Editor"})
	Gui.editor_group:append_row(Editing.dialog)
	Gui.menu_group = Widget{rows = 1}
	Gui.menu_group:append_col(Gui.menus)
	Gui.menu_group:append_col(Gui.chat_group)
	Gui.menu_group:set_expand{col = 2}
	Gui.center_group = Widget{cols = 1}
	Gui.center_group:append_row(Gui.fps_label)
	Gui.center_group:append_row(Gui.menu_group)
	Gui.center_group:append_row(Gui.skills_group)
	Gui.center_group:set_expand{row = 1}
	Gui.scene = Widgets.Scene{rows = 1, margins = {5,5,5,5}}
	Gui.scene:append_col(Gui.editor_group)
	Gui.scene:append_col(Gui.center_group)
	Gui.scene:set_expand{col = 2, row = 1}
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
