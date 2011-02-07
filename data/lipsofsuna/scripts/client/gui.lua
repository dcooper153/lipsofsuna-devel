Gui = Class()

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	Gui.skills = Views.Skills{main = true, sync = true}
	-- Admin menu.
	Gui.menu_widget_admin = Widgets.Menu{
		{"Editor", Editing.dialog},
		{"Save", function() Network:send{packet = Packet(packets.ADMIN_SAVE)} end},
		{"Shutdown", function() Network:send{packet = Packet(packets.ADMIN_SHUTDOWN)} end}}
	Gui.menu_widget_main = Widgets.Menu{
		{"Feats", function() clss:set_mode("feats", 2) end, arrow = true},
		{"Inventory", function() clss:set_mode("inventory", 2) end, arrow = true},
		{"Quests", function() clss:set_mode("quests", 2) end, arrow = true},
		{"Skills", function() clss:set_mode("skills", 2) end, arrow = true},
		{"-----", function() end},
		{"Admin", Gui.menu_widget_admin},
		{"Options", function() clss:set_mode("options", 2) end, arrow = true},
		{"Help", function() clss:set_mode("help", 2) end, arrow = true},
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
	local skillgrp = Widget{cols = 1, spacings = {0,0}}
	skillgrp:append_row(Gui.skill_health)
	skillgrp:append_row(Gui.skill_mana)
	-- Skills group.
	local pad = Widget()
	pad:set_request{width = 64}
	Gui.fps_label = Widgets.Label{valign = 1}
	Gui.skills_group = Widgets.Frame{rows = 1, style = "quickbar"}
	Gui.skills_group:append_col(skillgrp)
	Gui.skills_group:append_col(pad)
	Gui.skills_group:append_col(Quickslots.group)
	Gui.skills_group:append_col(Gui.fps_label)
	Gui.skills_group:set_expand{col = 3}
	-- Modifiers.
	Gui.modifiers = Widgets.Modifiers()
	-- Packing.
	Gui.top_group = Widget{rows = 1}
	Gui.top_group:append_col(Gui.chat_group)
	Gui.top_group:append_col(Gui.modifiers)
	Gui.top_group:set_expand{col = 1}
	Gui.center_group = Widget{cols = 1}
	Gui.center_group:append_row(Gui.top_group)
	Gui.center_group:append_row(Gui.menus)
	Gui.center_group:append_row(Gui.skills_group)
	Gui.center_group:set_expand{col = 1, row = 1}
	Gui.scene = Widgets.Scene{rows = 1, camera = Player.camera, margins = {5,5,5,0}}
	Gui.scene:append_col(Gui.center_group)
	Gui.scene:set_expand{col = 1, row = 1}
	Gui.bottom = Widget{rows = 1}
	Gui.bottom:append_col(Gui.scene)
	Gui.bottom:append_col(nil)
	Gui.bottom:set_expand{col = 1, row = 1}
	Gui.main = Widget{cols = 1, behind = true, fullscreen = true}
	Gui.main:append_row(Gui.bottom)
	Gui.main:set_expand{col = 1, row = 1}
end

Gui.set_mode = function(self, mode, level)
	-- Check for state changes.
	if self.mode == mode then return end
	-- Close the old view.
	Target:cancel()
	Drag:cancel()
	if self.view and self.view.close then self.view:close() end
	-- Set the base view mode.
	local base = (mode == "startup" or mode == "wait") and 1 or (mode == "chargen") and 2 or 3
	if base ~= self.mode_base then
		if self.mode_base == 3 then
			-- Disconnecting.
			for k,v in pairs(Object.objects) do
				v.realized = false
				Player.object = nil
			end
			self.main.floating = false
		end
		if base == 1 then
			-- Connection mode.
			self.mode_base = 1
			Startup.group.floating = true
			Sound:switch_music_track("menu")
		elseif mode == "chargen" then
			-- Character creation.
			self.mode_base = 2
			Views.Chargen.floating = true
			Sound:switch_music_track("char")
		else
			-- Game modes.
			self.mode_base = 3
			self.main.floating = true
			Program:unload_world()
			Sound:switch_music_track("game")
		end
	end
	-- Set the detailed view mode.
	self.mode = mode
	if mode == "chargen" then
		self.view = Views.Chargen.inst
	elseif mode == "dialog" then
		Gui.menus:open{level = level or 1, widget = Views.Dialog.inst}
		self.view = Views.Dialog.inst
	elseif mode == "feats" then
		Gui.menus:open{level = level or 1, widget = Views.Feats.inst}
		self.view = Views.Feats.inst
	elseif mode == "game" then
		Gui.menus:close()
		self.view = Views.Game.inst
	elseif mode == "help" then
		Gui.menus:open{level = level or 1, widget = Views.Help.inst}
		self.view = Views.Help.inst
	elseif mode == "inventory" then
		Gui.menus:open{level = level or 1, widget = Views.Inventory.inst}
		self.view = Views.Inventory.inst
	elseif mode == "menu" then
		Gui.menus:open{level = level or 1, widget = Gui.menu_widget_main}
		self.view = Gui.menu_widget_main
	elseif mode == "options" then
		Gui.menus:open{level = level or 1, widget = Views.Options.inst}
		self.view = Views.Options.inst
	elseif mode == "quests" then
		Gui.menus:open{level = level or 1, widget = Views.Quests.inst}
		self.view = Views.Quests.inst
	elseif mode == "skills" then
		Gui.menus:open{level = level or 1, widget = self.skills}
		self.view = self.skills
	elseif mode == "startup" then
		self.view = Startup
	end
	if self.view and self.view.enter then self.view:enter() end
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
