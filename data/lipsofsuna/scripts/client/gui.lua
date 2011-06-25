Gui = Class()

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	Gui.menus = Widgets.Menus()
	Gui.chat_history = Widgets.Log()
	-- Skill group.
	Gui.skill_health = Widgets.SkillControl{compact = true}
	Gui.skill_mana = Widgets.SkillControl{compact = true}
	local skillgrp = Widget{cols = 1, spacings = {0,0}}
	skillgrp:append_row(Gui.skill_health)
	skillgrp:append_row(Gui.skill_mana)
	-- Chat entry.
	Gui.chat_label = Widgets.Label{text = " "}
	Gui.chat_entry = Widgets.Entry{transparent = true, visible = false}
	Gui.chat_entry.pressed = function(self)
		Network:send{packet = Packet(packets.PLAYER_CHAT, "string", self.text)}
		self:clear()
	end
	Gui.chat_group = Widget{cols = 1, spacings = {0,0}}
	Gui.chat_group:append_row(Gui.chat_history)
	Gui.chat_group:set_expand{col = 1, row = 1}
	Gui.chat_group = Widget{cols = 2, rows = 1, spacings = {0, 0}}
	Gui.chat_group:set_child(1, 1, Gui.chat_label)
	Gui.chat_group:set_child(2, 1, Gui.chat_entry)
	Gui.chat_group:set_expand{col = 2}
	-- Skills group.
	local pad = Widget()
	pad:set_request{width = 64}
	Gui.fps_label = Widgets.Label{valign = 1}
	Gui.skills_group = Widgets.Frame{cols = 4, rows = 2, style = "quickbar"}
	Gui.skills_group:set_child{col = 1, row = 2, widget = skillgrp}
	Gui.skills_group:set_child{col = 2, row = 2, widget = pad}
	Gui.skills_group:set_child{col = 3, row = 1, widget = Gui.chat_group}
	Gui.skills_group:set_child{col = 3, row = 2, widget = Quickslots.group}
	Gui.skills_group:set_child{col = 4, row = 2, widget = Gui.fps_label}
	Gui.skills_group:set_expand{col = 3}
	-- Modifiers.
	Gui.modifiers = Widgets.Modifiers()
	-- Respawning.
	Gui.button_respawn = Widgets.Button{font = "medium", text = "Create a new character",
		pressed = function() Network:send{packet = Packet(packets.PLAYER_RESPAWN)} end}
	Gui.group_respawn = Widget{cols = 1, rows = 3, margins = {0,0,5,0}, visible = false}
	Gui.group_respawn:set_expand{col = 1, row = 1}
	Gui.group_respawn:set_expand{row = 3}
	Gui.group_respawn:set_child(1, 1, Gui.button_respawn)
	Gui.group_respawn:set_child(1, 2, Gui.button_respawn)
	-- Dialog.
	Gui.group_dialog = Widget{cols = 1, rows = 3}
	Gui.group_dialog:set_expand{row = 1}
	Gui.group_dialog:set_child(1, 2, Widgets.Label())
	-- Packing.
	Gui.group_top = Widget{cols = 3, rows = 1}
	Gui.group_top:set_expand{col = 1}
	Gui.group_top:set_child(1, 1, Gui.chat_history)
	Gui.group_top:set_child(2, 1, Gui.modifiers)
	Gui.group_top:set_child(3, 1, Gui.group_respawn)
	Gui.group_middle = Widget{cols = 2, rows = 1}
	Gui.group_middle:set_expand{col = 1, row = 1}
	Gui.group_middle:set_child(1, 1, Gui.menus)
	Gui.group_middle:set_child(2, 1, Gui.group_dialog)
	Gui.scene = Widgets.Scene{cols = 1, rows = 3, margins = {5,5,0,0}, spacings = {0,0}}
	Gui.scene:set_expand{col = 1, row = 1}
	Gui.scene:set_child(1, 1, Gui.group_top)
	Gui.scene:set_child(1, 2, Gui.group_middle)
	Gui.scene:set_child(1, 3, Gui.skills_group)
	Gui.main = Widget{cols = 1, behind = true, fullscreen = true}
	Gui.main:append_row(Gui.scene)
	Gui.main:set_expand{col = 1, row = 1}
end

Gui.set_dead = function(self, value)
	if self.group_respawn.visible == value then return end
	if value then
		self.modifiers.visible = false
		self.group_respawn.visible = true
	else
		self.group_respawn.visible = false
		self.modifiers.visible = true
	end
end

--- Sets or unsets the text of the action label.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_action_text = function(clss, text)
	clss.scene.action = text
end

--- Sets the ID of the object whose dialog is shown.
-- @param self Gui class.
-- @param id Object ID.
Gui.set_dialog = function(clss, id)
	-- Find the dialog.
	clss.active_dialog = id
	local obj = id and Object:find{id = id}
	local dlg = obj and obj.dialog
	-- Update the dialog UI.
	if not dlg then
		-- Hide the dialog UI.
		clss.dialog_choices = nil
		clss.group_dialog.rows = 1
		clss.group_dialog.visible = false
	elseif dlg.type == "choice" then
		-- Show a dialog choice.
		clss.dialog_choices = {}
		clss.group_dialog.rows = 1
		for k,v in ipairs(dlg.choices) do
			local widget = Widgets.DialogLabel{choice = true, index = k, text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end}
			table.insert(clss.dialog_choices, widget)
			clss.group_dialog:append_row(widget)
		end
		clss.group_dialog.visible = true
	else
		-- Show a dialog line.
		local widget = Widgets.DialogLabel{index = 1, text = dlg.message, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")}
		end}
		clss.dialog_choices = {widget}
		clss.group_dialog.rows = 1
		clss.group_dialog:append_row(widget)
		clss.group_dialog.visible = true
	end
end

--- Sets or unsets the active target.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_target_text = function(clss, text)
	clss.scene.action = text
end

Gui.class_getters = {
	chat_active = function(s) return s.chat_entry.visible end}

Gui.class_setters = {
	chat_active = function(s, v)
		s.chat_entry.visible = v
		s.chat_label.text = v and "TALK: " or " "
		Client:set_mode("game")
	end}
